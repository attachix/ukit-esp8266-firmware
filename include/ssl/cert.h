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

unsigned char default_certificate[] = {
  0x30, 0x82, 0x03, 0x74, 0x30, 0x82, 0x02, 0x5c, 0xa0, 0x03, 0x02, 0x01,
  0x02, 0x02, 0x02, 0x70, 0x03, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48,
  0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x74, 0x31, 0x0b,
  0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x44, 0x45, 0x31,
  0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x02, 0x42, 0x57,
  0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x0b, 0x53,
  0x4c, 0x4b, 0x20, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6d, 0x73, 0x31, 0x1b,
  0x30, 0x19, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x12, 0x69, 0x6d, 0x2e,
  0x63, 0x61, 0x2e, 0x61, 0x74, 0x74, 0x61, 0x63, 0x68, 0x69, 0x78, 0x2e,
  0x63, 0x6f, 0x6d, 0x31, 0x25, 0x30, 0x23, 0x06, 0x09, 0x2a, 0x86, 0x48,
  0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01, 0x16, 0x16, 0x77, 0x65, 0x62, 0x6d,
  0x61, 0x73, 0x74, 0x65, 0x72, 0x40, 0x61, 0x74, 0x74, 0x61, 0x63, 0x68,
  0x69, 0x78, 0x2e, 0x63, 0x6f, 0x6d, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x37,
  0x30, 0x37, 0x32, 0x30, 0x31, 0x32, 0x34, 0x39, 0x32, 0x39, 0x5a, 0x17,
  0x0d, 0x31, 0x38, 0x30, 0x37, 0x32, 0x30, 0x31, 0x32, 0x34, 0x39, 0x32,
  0x39, 0x5a, 0x30, 0x81, 0x8b, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
  0x04, 0x06, 0x13, 0x02, 0x44, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03,
  0x55, 0x04, 0x08, 0x0c, 0x02, 0x42, 0x57, 0x31, 0x12, 0x30, 0x10, 0x06,
  0x03, 0x55, 0x04, 0x07, 0x0c, 0x09, 0x53, 0x74, 0x75, 0x74, 0x74, 0x67,
  0x61, 0x72, 0x74, 0x31, 0x15, 0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0a,
  0x0c, 0x0c, 0x41, 0x74, 0x74, 0x61, 0x63, 0x68, 0x69, 0x78, 0x2e, 0x63,
  0x6f, 0x6d, 0x31, 0x1d, 0x30, 0x1b, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c,
  0x14, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x78, 0x2e, 0x61, 0x74, 0x74,
  0x61, 0x63, 0x68, 0x69, 0x78, 0x2e, 0x63, 0x6f, 0x6d, 0x31, 0x25, 0x30,
  0x23, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x09, 0x01,
  0x16, 0x16, 0x77, 0x65, 0x62, 0x6d, 0x61, 0x73, 0x74, 0x65, 0x72, 0x40,
  0x61, 0x74, 0x74, 0x61, 0x63, 0x68, 0x69, 0x78, 0x2e, 0x63, 0x6f, 0x6d,
  0x30, 0x81, 0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7,
  0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8d, 0x00, 0x30, 0x81,
  0x89, 0x02, 0x81, 0x81, 0x00, 0xef, 0x6c, 0x19, 0xe0, 0x0d, 0x2a, 0xd6,
  0x5b, 0xd8, 0x8f, 0x22, 0x01, 0x95, 0x7e, 0x82, 0xea, 0x51, 0x5c, 0x3b,
  0x8d, 0xa7, 0xf9, 0x7e, 0x1b, 0xed, 0x7d, 0x7e, 0xdf, 0xbc, 0xbd, 0xc9,
  0x62, 0x6f, 0xdb, 0xf9, 0x76, 0xa6, 0x77, 0x43, 0x52, 0x9c, 0x75, 0x28,
  0x35, 0x43, 0x3e, 0x47, 0xf2, 0xe5, 0xfb, 0x9c, 0x28, 0x47, 0x68, 0x22,
  0x18, 0x0e, 0xab, 0x17, 0xbf, 0x10, 0x7e, 0xed, 0x5c, 0xc8, 0xd0, 0x97,
  0xc4, 0x76, 0xe3, 0xe1, 0x70, 0x34, 0xbf, 0xe0, 0xa6, 0xb4, 0xd7, 0xf1,
  0x6c, 0x23, 0x8d, 0x66, 0x94, 0x74, 0x3c, 0x67, 0xc0, 0xc2, 0x99, 0x02,
  0x94, 0xf8, 0x2d, 0xa5, 0x44, 0x0e, 0x18, 0x48, 0xe8, 0x8f, 0x72, 0xc6,
  0x3f, 0xaa, 0x6c, 0x7e, 0x04, 0xe5, 0x7a, 0xdd, 0xa7, 0x74, 0x22, 0x26,
  0x19, 0x2f, 0x35, 0x08, 0x3e, 0x7a, 0xdb, 0x2c, 0x35, 0xd9, 0xe6, 0x84,
  0x15, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x7c, 0x30, 0x7a, 0x30, 0x09,
  0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 0x1d, 0x06,
  0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x65, 0xa1, 0xc3, 0x79,
  0xe4, 0xb1, 0x62, 0xb1, 0x77, 0x05, 0x52, 0x8e, 0x64, 0xd9, 0x95, 0x35,
  0xc9, 0x19, 0xea, 0xb3, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04,
  0x18, 0x30, 0x16, 0x80, 0x14, 0x78, 0x32, 0x05, 0x91, 0x55, 0x8f, 0x68,
  0x96, 0xf1, 0x92, 0xac, 0x89, 0x71, 0x97, 0xd2, 0x90, 0xab, 0x93, 0x75,
  0xfc, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04,
  0x04, 0x03, 0x02, 0x05, 0xe0, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x25,
  0x04, 0x16, 0x30, 0x14, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07,
  0x03, 0x02, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x04,
  0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
  0x0b, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0x2d, 0x5a, 0xe9, 0xce,
  0xbf, 0xae, 0x9b, 0xd9, 0x8b, 0xa4, 0x51, 0x74, 0x23, 0x8c, 0xf3, 0x02,
  0x8d, 0x3b, 0x5c, 0x4f, 0x59, 0x61, 0xc7, 0x7f, 0x5d, 0x05, 0xde, 0xb0,
  0xb0, 0xe7, 0x7c, 0x55, 0x47, 0xbe, 0xd9, 0x38, 0x59, 0x42, 0xeb, 0xa5,
  0xfe, 0x4d, 0x8e, 0xa5, 0x85, 0x61, 0x86, 0xce, 0xbe, 0x98, 0x2c, 0x25,
  0x39, 0x62, 0x0b, 0xd2, 0x8c, 0x27, 0x33, 0x58, 0xbf, 0x91, 0x68, 0x53,
  0x38, 0x63, 0xa1, 0xea, 0xe3, 0x23, 0x10, 0x1f, 0xe1, 0xf0, 0x85, 0x28,
  0xcb, 0x61, 0x06, 0xab, 0xb9, 0xb9, 0x97, 0x30, 0xe1, 0xdb, 0xbd, 0x5c,
  0xb7, 0x30, 0xce, 0x79, 0xef, 0x66, 0xab, 0xab, 0x8c, 0x7e, 0xc1, 0x7d,
  0x28, 0x01, 0x53, 0xf6, 0x5b, 0x6b, 0xc2, 0xad, 0x2a, 0xc3, 0xdf, 0x84,
  0x71, 0x34, 0xc2, 0xee, 0x97, 0x54, 0xa9, 0xd5, 0xfc, 0x30, 0xd5, 0x99,
  0x6c, 0x01, 0xed, 0x6b, 0xf7, 0x2c, 0x5c, 0x90, 0x53, 0xdd, 0x14, 0xad,
  0x44, 0x65, 0x37, 0xbd, 0x54, 0xe0, 0xcf, 0x9d, 0x3f, 0x86, 0xb2, 0xd2,
  0x50, 0xa9, 0x73, 0x4b, 0x8a, 0x72, 0x6d, 0x3a, 0x67, 0x5c, 0x30, 0x8e,
  0xe6, 0x2b, 0xe8, 0x9f, 0xff, 0xc8, 0x62, 0x01, 0x36, 0xc4, 0xcb, 0xaa,
  0xef, 0xd1, 0xe1, 0x39, 0xe9, 0xdc, 0x4f, 0x05, 0x99, 0x8a, 0x7d, 0x87,
  0x0c, 0x9c, 0x96, 0x17, 0xb1, 0x69, 0x9a, 0x9b, 0x00, 0xbd, 0xff, 0xe4,
  0x72, 0x78, 0xb2, 0x98, 0xbb, 0xfa, 0xc9, 0x32, 0x02, 0x6d, 0xc2, 0x7c,
  0x3b, 0x9a, 0xfc, 0x03, 0x59, 0xd5, 0xc8, 0xea, 0xea, 0x79, 0xc0, 0x78,
  0xbd, 0xe4, 0xc9, 0x8d, 0xb3, 0x32, 0x18, 0x33, 0x6d, 0x43, 0x21, 0xcf,
  0x11, 0x60, 0x63, 0xcd, 0xe2, 0xba, 0x85, 0x90, 0x2d, 0xa1, 0xcc, 0xef,
  0x94, 0xe3, 0xb3, 0x7e, 0x23, 0xda, 0x63, 0x07, 0x60, 0x2c, 0x6f, 0x43
};
unsigned int default_certificate_len = 888;
