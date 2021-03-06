/*
 * Copyright (C) 2019 Rick V. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Entry point for Loki Pager text-mode client for Loki communications network */
#include "app.h"
/* Global variables and fixed app-specific data */
char* XCursesProgramName = "Session Pager";

static bool http_start = false;
static bool signal_start = false;
static signal_context* loki_signal_ctx;
static signal_protocol_store_context* loki_store_context;
extern signal_crypto_provider mbedtls_signal_csp;

static bool boot_signal()
{
	int r;
#ifdef _WIN32
	/* 64-bit time_t on windows since 1999 ffs */
	__time64_t timestamp;
	InitializeCriticalSection(&global_mutex);
	_time64(&timestamp);
#else
	time_t timestamp;
	time(&timestamp);
#endif
	r = signal_context_create(&loki_signal_ctx, NULL);
	if (r)
		return false;
	r = signal_context_set_crypto_provider(loki_signal_ctx, &mbedtls_signal_csp);
	if (r)
		return false;
	r = signal_context_set_locking_functions(loki_signal_ctx, loki_lock, loki_unlock);
	if (r)
		return false;
	r = signal_context_set_log_function(loki_signal_ctx, loki_log);
	if (r)
		return false;
	new_user_ctx = malloc(sizeof(loki_user_ctx));
	if (!new_user_ctx)
		return false;
	setup_loki_store_context(&loki_store_context, loki_signal_ctx);
	mbedtls_platform_zeroize(new_user_ctx, sizeof(loki_user_ctx));
	r = signal_protocol_key_helper_generate_identity_key_pair(&new_user_ctx->identity_key_pair, loki_signal_ctx);
	if (r)
		return false;
	r = signal_protocol_key_helper_generate_registration_id(&new_user_ctx->registration_id, 0, loki_signal_ctx);
	if (r)
		return false;
	r = signal_protocol_key_helper_generate_pre_keys(&new_user_ctx->pre_keys_head, 1, 100, loki_signal_ctx);
	if (r)
		return false;
	r = signal_protocol_key_helper_generate_signed_pre_key(&new_user_ctx->signed_pre_key, new_user_ctx->identity_key_pair, 5, timestamp, loki_signal_ctx);
	if (r)
		return false;
	return true;
}

/* Here we extract the newly-generated keys, then
 * generate the hex-encoded and Base64 encoded variants
 * and write them back
 */
static void extract_identity_keys()
{
	unsigned char* data;
	ec_public_key* pub;
	ec_private_key* secret;

	pub = ratchet_identity_key_pair_get_public(new_user_ctx->identity_key_pair);
	secret = ratchet_identity_key_pair_get_private(new_user_ctx->identity_key_pair);
	ec_public_key_serialize(&new_user_ctx->pub_key, pub);
	ec_private_key_serialize(&new_user_ctx->secret_key, secret);
	data = signal_buffer_data(new_user_ctx->pub_key);
	printPubHex(new_user_ctx->pubHex, data);
	new_user_ctx->pubB64 = bufferToBase64(data, 33);
	data = signal_buffer_data(new_user_ctx->secret_key);
	printSecretHex(new_user_ctx->secretHex, data);
	new_user_ctx->secretB64 = bufferToBase64(data, 32);
}

/* scrub everything when we exit, in case this key has been exposed
 * to secondary storage
 */
static void scrub_keys()
{
	signal_buffer_bzero_free(new_user_ctx->pub_key);
	signal_buffer_bzero_free(new_user_ctx->secret_key);
	mbedtls_platform_zeroize(new_user_ctx->pubHex, 65);
	mbedtls_platform_zeroize(new_user_ctx->secretHex, 65);
	signal_protocol_key_helper_key_list_free(new_user_ctx->pre_keys_head);
	mbedtls_platform_zeroize(new_user_ctx->pubB64, strlen(new_user_ctx->pubB64));
	mbedtls_platform_zeroize(new_user_ctx->secretB64, strlen(new_user_ctx->secretB64));
	free(new_user_ctx->pubB64);
	free(new_user_ctx->secretB64);
	mbedtls_platform_zeroize(new_user_ctx, sizeof(loki_user_ctx));
	free(new_user_ctx);
}

main(argc, argv)
char** argv;
{
	int status;
	CDK_PARAMS params;
	enum RESULT r;
	char* window_text[1];

	CDKparseParams(argc, argv, &params, "s:" CDK_CLI_PARAMS);

	/* start http and base signal protocol */
	http_start = http_client_init();
	signal_start = boot_signal();

	if (!http_start)
		return -1;

	if (!signal_start)
		return -1;

	/* Start curses. */
	cdkscreen = initCDKScreen(NULL);
	initCDKColor();
	curs_set(0);
	window_text[0] = "Welcome to Session Pager";

	/* title bar */
	title = newCDKLabel(cdkscreen, CENTER, 0,
		(CDK_CSTRING2)window_text, 1,
		FALSE, FALSE);

	/* some base UI colours. these will get reassigned
	 * once we get to the chat window 
	 */
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_RED);

	/* Box our window. */
	box(stdscr, ACS_VLINE, ACS_HLINE);

	/* TODO(despair): load settings from sqlite and
	 * let the user specify default language
	 */
	w = initialise_wordlist(LANGUAGE_DEFAULT);
	if (!w)
		fatal_error("Could not load default mnemonic wordlist. Is Session installed properly?");
	/* Display the first window */
	splash();
	refreshCDKScreen(cdkscreen);
#ifndef _EXPORT_BUILD
	export_warning();
#endif
	r = create_or_restore_seed();
	switch (r)
	{
	case RESTORE_EXISTING_SEED:
		restore_seed();
		break;
	case CREATE_NEW_SEED:
		extract_identity_keys();
		new_user();
		scrub_keys();
		break;
	default:
		break;
	};

	http_client_cleanup();

	destroyCDKLabel(title);
	destroyCDKScreen(cdkscreen);
	endCDK();
	signal_context_destroy(loki_signal_ctx);
	destroy_wordlist(w);
	status = 0;
#ifdef _WIN32
	DeleteCriticalSection(&global_mutex);
	OpenClipboard(NULL);
	EmptyClipboard();
	CloseClipboard();
#endif
	return status;
}
