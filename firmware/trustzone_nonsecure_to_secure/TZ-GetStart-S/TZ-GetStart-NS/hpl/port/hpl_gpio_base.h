
/**
 * \file
 *
 * \brief SAM PORT.
 *
 * Copyright (c) 2017-2018 Microchip Technology Inc. and its subsidiaries.
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
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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
#include <compiler.h>
#include <hpl_gpio.h>
#include <utils_assert.h>

#if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
#define _PORT PORT_SEC
#define _PORT_IOBUS PORT_IOBUS_SEC
#else
#define _PORT PORT
#define _PORT_IOBUS PORT_IOBUS
#endif

/**
 * \brief Set direction on port with mask
 */
static inline void _gpio_set_direction(const enum gpio_port port, const uint32_t mask,
                                       const enum gpio_direction direction)
{
	switch (direction) {
	case GPIO_DIRECTION_OFF:
		hri_port_clear_DIR_reg(_PORT_IOBUS, port, mask);
		hri_port_write_WRCONFIG_reg(_PORT, port, PORT_WRCONFIG_WRPINCFG | (mask & 0xffff));
		hri_port_write_WRCONFIG_reg(
		    _PORT, port, PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | ((mask & 0xffff0000) >> 16));
		break;

	case GPIO_DIRECTION_IN:
		hri_port_clear_DIR_reg(_PORT_IOBUS, port, mask);
		hri_port_write_WRCONFIG_reg(_PORT, port, PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN | (mask & 0xffff));
		hri_port_write_WRCONFIG_reg(_PORT,
		                            port,
		                            PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | PORT_WRCONFIG_INEN
		                                | ((mask & 0xffff0000) >> 16));
		break;

	case GPIO_DIRECTION_OUT:
		hri_port_set_DIR_reg(_PORT_IOBUS, port, mask);
		hri_port_write_WRCONFIG_reg(_PORT, port, PORT_WRCONFIG_WRPINCFG | (mask & 0xffff));
		hri_port_write_WRCONFIG_reg(
		    _PORT, port, PORT_WRCONFIG_HWSEL | PORT_WRCONFIG_WRPINCFG | ((mask & 0xffff0000) >> 16));
		break;

	default:
		ASSERT(false);
	}
}

/**
 * \brief Set output level on port with mask
 */
static inline void _gpio_set_level(const enum gpio_port port, const uint32_t mask, const bool level)
{
	if (level) {
		hri_port_set_OUT_reg(_PORT_IOBUS, port, mask);
	} else {
		hri_port_clear_OUT_reg(_PORT_IOBUS, port, mask);
	}
}

/**
 * \brief Change output level to the opposite with mask
 */
static inline void _gpio_toggle_level(const enum gpio_port port, const uint32_t mask)
{
	hri_port_toggle_OUT_reg(_PORT_IOBUS, port, mask);
}

/**
 * \brief Get input levels on all port pins
 */
static inline uint32_t _gpio_get_level(const enum gpio_port port)
{
	uint32_t tmp;

	CRITICAL_SECTION_ENTER();

	uint32_t dir_tmp = hri_port_read_DIR_reg(_PORT_IOBUS, port);

	tmp = hri_port_read_IN_reg(_PORT, port) & ~dir_tmp;
	tmp |= hri_port_read_OUT_reg(_PORT_IOBUS, port) & dir_tmp;

	CRITICAL_SECTION_LEAVE();

	return tmp;
}

/**
 * \brief Set pin pull mode
 */
static inline void _gpio_set_pin_pull_mode(const enum gpio_port port, const uint8_t pin,
                                           const enum gpio_pull_mode pull_mode)
{
	switch (pull_mode) {
	case GPIO_PULL_OFF:
		hri_port_clear_PINCFG_PULLEN_bit(_PORT, port, pin);
		break;

	case GPIO_PULL_UP:
		hri_port_clear_DIR_reg(_PORT_IOBUS, port, 1U << pin);
		hri_port_set_PINCFG_PULLEN_bit(_PORT, port, pin);
		hri_port_set_OUT_reg(_PORT_IOBUS, port, 1U << pin);
		break;

	case GPIO_PULL_DOWN:
		hri_port_clear_DIR_reg(_PORT_IOBUS, port, 1U << pin);
		hri_port_set_PINCFG_PULLEN_bit(_PORT, port, pin);
		hri_port_clear_OUT_reg(_PORT_IOBUS, port, 1U << pin);
		break;

	default:
		ASSERT(false);
		break;
	}
}

/**
 * \brief Set gpio pin function
 */
static inline void _gpio_set_pin_function(const uint32_t gpio, const uint32_t function)
{
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	if (function == GPIO_PIN_FUNCTION_OFF) {
		hri_port_write_PINCFG_PMUXEN_bit(_PORT, port, pin, false);

	} else {
		hri_port_write_PINCFG_PMUXEN_bit(_PORT, port, pin, true);

		if (pin & 1) {
			// Odd numbered pin
			hri_port_write_PMUX_PMUXO_bf(_PORT, port, pin >> 1, function & 0xffff);
		} else {
			// Even numbered pin
			hri_port_write_PMUX_PMUXE_bf(_PORT, port, pin >> 1, function & 0xffff);
		}
	}
}

#if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
/**
 * \brief Set gpio pin non-secure attribution
 */
static inline void _gpio_set_pin_nonsec(const uint32_t gpio, bool non_sec)
{
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	if (non_sec) {
		hri_port_set_NONSEC_reg(_PORT, port, 0x1U << pin);
	} else {
		hri_port_clear_NONSEC_reg(_PORT, port, 0x1U << pin);
	}
}
#endif
