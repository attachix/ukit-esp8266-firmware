/* * U:Kit ESP8266 Firmware - This is the 'smart' firmware for the U:Kit sensor kit
 * Copyright (C) 2016, 2018 Slavey Karadzhov <slav@attachix.com>
 *
 * This file is part of U:Kit ESP8266 Firmware.
 *
 * U:Kit ESP8266 Firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * U:Kit ESP8266 Firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with U:Kit ESP8266 Firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

const unsigned char default_private_key[] = {
  0x30, 0x82, 0x02, 0x5d, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xef,
  0x6c, 0x19, 0xe0, 0x0d, 0x2a, 0xd6, 0x5b, 0xd8, 0x8f, 0x22, 0x01, 0x95,
  0x7e, 0x82, 0xea, 0x51, 0x5c, 0x3b, 0x8d, 0xa7, 0xf9, 0x7e, 0x1b, 0xed,
  0x7d, 0x7e, 0xdf, 0xbc, 0xbd, 0xc9, 0x62, 0x6f, 0xdb, 0xf9, 0x76, 0xa6,
  0x77, 0x43, 0x52, 0x9c, 0x75, 0x28, 0x35, 0x43, 0x3e, 0x47, 0xf2, 0xe5,
  0xfb, 0x9c, 0x28, 0x47, 0x68, 0x22, 0x18, 0x0e, 0xab, 0x17, 0xbf, 0x10,
  0x7e, 0xed, 0x5c, 0xc8, 0xd0, 0x97, 0xc4, 0x76, 0xe3, 0xe1, 0x70, 0x34,
  0xbf, 0xe0, 0xa6, 0xb4, 0xd7, 0xf1, 0x6c, 0x23, 0x8d, 0x66, 0x94, 0x74,
  0x3c, 0x67, 0xc0, 0xc2, 0x99, 0x02, 0x94, 0xf8, 0x2d, 0xa5, 0x44, 0x0e,
  0x18, 0x48, 0xe8, 0x8f, 0x72, 0xc6, 0x3f, 0xaa, 0x6c, 0x7e, 0x04, 0xe5,
  0x7a, 0xdd, 0xa7, 0x74, 0x22, 0x26, 0x19, 0x2f, 0x35, 0x08, 0x3e, 0x7a,
  0xdb, 0x2c, 0x35, 0xd9, 0xe6, 0x84, 0x15, 0x02, 0x03, 0x01, 0x00, 0x01,
  0x02, 0x81, 0x81, 0x00, 0xbe, 0xf5, 0x5a, 0x81, 0xf0, 0x1e, 0x9d, 0x8f,
  0xed, 0xde, 0xe3, 0x81, 0x9d, 0xe9, 0xa4, 0xa3, 0x1d, 0x37, 0x52, 0x2c,
  0x7e, 0xb0, 0x28, 0x9c, 0x3f, 0xc4, 0x5b, 0x60, 0x3a, 0x1a, 0xa9, 0x37,
  0xc0, 0xfe, 0x13, 0x2f, 0x46, 0x40, 0x53, 0x4a, 0xdd, 0x8d, 0x37, 0xff,
  0x9b, 0x5d, 0x5e, 0x67, 0x66, 0x75, 0x53, 0x2c, 0x71, 0x1f, 0xa6, 0x29,
  0xc7, 0xf1, 0x40, 0x64, 0x5c, 0xd4, 0xcf, 0x9d, 0x29, 0x59, 0x29, 0xe2,
  0x4c, 0x37, 0xe7, 0x6e, 0xe3, 0xaa, 0x7f, 0xc7, 0x4a, 0x03, 0x98, 0x85,
  0x86, 0x04, 0x85, 0x13, 0x56, 0xcf, 0x95, 0x94, 0xec, 0xc8, 0x5f, 0x81,
  0xb9, 0x99, 0x9f, 0xe8, 0x0c, 0xb2, 0x1b, 0x27, 0x1f, 0x1d, 0xa0, 0xd0,
  0x07, 0x7a, 0x32, 0xca, 0x36, 0xf6, 0x9a, 0x65, 0xc2, 0xfc, 0xb1, 0xed,
  0xfb, 0xd6, 0xf1, 0x52, 0xd9, 0x79, 0xe8, 0xc7, 0x5c, 0x11, 0xd7, 0x01,
  0x02, 0x41, 0x00, 0xff, 0x03, 0x0b, 0xb4, 0x98, 0x62, 0x9a, 0x41, 0xe2,
  0x65, 0x75, 0xfb, 0x5a, 0x7b, 0x1b, 0x48, 0xa5, 0x1a, 0x74, 0xd0, 0x9b,
  0xec, 0x59, 0xcc, 0x5d, 0xe5, 0xca, 0xc2, 0x1a, 0x05, 0x84, 0xbd, 0x52,
  0x22, 0xd6, 0x4c, 0x40, 0xd2, 0x58, 0x04, 0x22, 0xb5, 0xd1, 0x17, 0xef,
  0x9c, 0xb8, 0xee, 0x7b, 0xec, 0x20, 0xe7, 0x32, 0x43, 0xcc, 0x28, 0x53,
  0xbe, 0x07, 0xe0, 0x39, 0x2e, 0x15, 0xd5, 0x02, 0x41, 0x00, 0xf0, 0x59,
  0x97, 0x6d, 0x4c, 0x9d, 0x9d, 0xf3, 0xb0, 0x0d, 0x20, 0x4f, 0x92, 0xe7,
  0xcc, 0x0a, 0x10, 0x2e, 0x2d, 0x69, 0xbb, 0x05, 0x54, 0xa3, 0x78, 0x24,
  0x53, 0x5c, 0xb6, 0xbc, 0x10, 0xa1, 0x64, 0xbf, 0x4e, 0x55, 0x52, 0x18,
  0x6a, 0x85, 0x99, 0x6c, 0x09, 0x14, 0xb3, 0x4b, 0xd3, 0xc8, 0xa6, 0x4f,
  0xd8, 0x72, 0xa9, 0x4f, 0xe3, 0x90, 0x74, 0x99, 0x65, 0x81, 0x41, 0x86,
  0x95, 0x41, 0x02, 0x41, 0x00, 0xf1, 0x62, 0x9d, 0xaa, 0xe5, 0xef, 0xb7,
  0xda, 0x93, 0x85, 0xbc, 0xe7, 0x1f, 0x1f, 0x96, 0x94, 0xee, 0x0b, 0x9b,
  0xf5, 0x77, 0x12, 0x83, 0x71, 0xca, 0x93, 0xe0, 0x98, 0x58, 0xef, 0x4e,
  0x55, 0xb2, 0xb0, 0xdd, 0x2e, 0xb2, 0xd1, 0x28, 0x95, 0x49, 0x91, 0xd7,
  0x09, 0x94, 0x3b, 0x42, 0x66, 0x24, 0x11, 0x40, 0x97, 0xf7, 0xe1, 0x6b,
  0x17, 0xe5, 0x24, 0xe4, 0xe9, 0x9e, 0xd6, 0x7b, 0xbd, 0x02, 0x40, 0x26,
  0xb1, 0xd5, 0xce, 0x33, 0xf2, 0x80, 0x36, 0x76, 0x56, 0x76, 0xa8, 0x38,
  0xf7, 0x76, 0x82, 0xfc, 0x3d, 0xe2, 0xf3, 0xcd, 0xb7, 0x3a, 0xfc, 0x22,
  0x60, 0x7e, 0xc7, 0x43, 0x68, 0x1d, 0xa9, 0x4e, 0x25, 0xfa, 0x7f, 0x83,
  0xfb, 0x08, 0x53, 0x3f, 0xf3, 0x03, 0xb7, 0x40, 0x72, 0x69, 0xc3, 0x38,
  0xfb, 0x4b, 0xd6, 0x91, 0xe9, 0x99, 0xb2, 0x6d, 0x03, 0x31, 0x33, 0x51,
  0x10, 0x50, 0xc1, 0x02, 0x40, 0x42, 0xe3, 0x33, 0x96, 0x27, 0x5f, 0x98,
  0x79, 0x6a, 0x12, 0x4b, 0xa2, 0x82, 0x0a, 0x80, 0xa7, 0x71, 0x84, 0xd5,
  0xb2, 0xa4, 0x9d, 0xb1, 0x20, 0xb5, 0xd0, 0x29, 0x2a, 0x20, 0xb3, 0x06,
  0x64, 0xc8, 0x69, 0x4b, 0x6b, 0xf7, 0x5f, 0x83, 0x6c, 0x20, 0x3c, 0x66,
  0x80, 0xb4, 0xbc, 0xf4, 0x3c, 0x32, 0x8b, 0x85, 0x6b, 0xe6, 0x31, 0x63,
  0x3a, 0x91, 0x8a, 0x3e, 0xb4, 0x5a, 0x81, 0xd0, 0x92
};
unsigned int default_private_key_len = 609;
