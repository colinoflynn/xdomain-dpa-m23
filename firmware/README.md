These directories contain the firmware source code used for the paper. These firmware builds require a Segger J-Link to offload data, as
they use the RTT function.

They are built using Atmel Studio, and based on the ChipWhisperer HAL for the SAML11.

There are two directories - one contains a "non-secure to secure" code, one contains "secure only" code.

## trustzone_nonsecure_to_secure

This builds two firmware files (as required by the TrustZone-M system), one for the secure core and one for the non-secure core. There is then a small veneer used for calling between functions. This firmware is the one used to demonstrate the cross-domain attack, since the ADC is started before calling the AES routine (which happens in the secure domain).

In general, downloading and debugging the non-secure to secure code is a hassle. For this reason there is a secure-only version.

## secure_only

This builds a firmware file that only uses the secure mode. This is generally much easier to work with than the true cross-domain code. By first validating that the cross-domain code and secure-only code produce equivalent results, we can be confident that a non-secure peripheral started before switching to secure mode is *not* stopped.

Thus we can use the secure_only code for the majority of this work.
