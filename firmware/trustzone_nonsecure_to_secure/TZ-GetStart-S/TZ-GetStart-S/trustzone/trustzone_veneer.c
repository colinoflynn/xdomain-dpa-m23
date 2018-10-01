/**
 * \file
 *
 * \brief TrustZone secure gateway veneer implementation.
 *
 * Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#include "atmel_start.h"
#include "trustzone_veneer.h"
#include "trustzone_config.h"

/* ======== Non-secure callable functions for Getting Started Example ======== */
typedef void (*IDAU_AES_FUNC)(const uint8_t *keys, uint32_t key_len,
const uint8_t *src, uint8_t *dst);
IDAU_AES_FUNC idau_aes_enc = (IDAU_AES_FUNC)(IDAU_CRYA_AES_ENCRYPT_T | 0x01);
IDAU_AES_FUNC idau_aes_dec = (IDAU_AES_FUNC)(IDAU_CRYA_AES_DECRYPT_T | 0x01);

/*
 * \brief Non-secure callable function 1
 */
void __attribute__((cmse_nonsecure_entry)) nsc_func_enc(const uint8_t *keys, uint32_t key_len, const uint8_t *src, uint8_t *dst)
{
	return idau_aes_enc(keys, key_len, src, dst);
}

/*
 * \brief Non-secure callable function 2
 */
void __attribute__((cmse_nonsecure_entry)) nsc_func_dec(const uint8_t *keys, uint32_t key_len, const uint8_t *src, uint8_t *dst)
{
	return idau_aes_enc(keys, key_len, src, dst);
}

/* ======== Non-secure callable functions defined by customer ======== */
/*
 * You may add your secure gateway veneer functions in this file
 * and the function declaration into trustzone_veneer.h.
 */
