// Created by http://oleddisplay.squix.ch/ Consider a donation
// In case of problems make sure that you are using the font file with the correct version!
const uint8_t Monospaced_plain_16[] PROGMEM = {
  0x0A, // Width: 10
  0x13, // Height: 19
  0x20, // First Char: 32
  0xE0, // Numbers of Chars: 224

  // Jump Table:
  0xFF, 0xFF, 0x00, 0x0A,  // 32:65535
  0x00, 0x00, 0x0E, 0x0A,  // 33:0
  0x00, 0x0E, 0x13, 0x0A,  // 34:14
  0x00, 0x21, 0x1C, 0x0A,  // 35:33
  0x00, 0x3D, 0x1A, 0x0A,  // 36:61
  0x00, 0x57, 0x1A, 0x0A,  // 37:87
  0x00, 0x71, 0x1A, 0x0A,  // 38:113
  0x00, 0x8B, 0x0D, 0x0A,  // 39:139
  0x00, 0x98, 0x15, 0x0A,  // 40:152
  0x00, 0xAD, 0x11, 0x0A,  // 41:173
  0x00, 0xBE, 0x17, 0x0A,  // 42:190
  0x00, 0xD5, 0x17, 0x0A,  // 43:213
  0x00, 0xEC, 0x11, 0x0A,  // 44:236
  0x00, 0xFD, 0x14, 0x0A,  // 45:253
  0x01, 0x11, 0x11, 0x0A,  // 46:273
  0x01, 0x22, 0x16, 0x0A,  // 47:290
  0x01, 0x38, 0x1A, 0x0A,  // 48:312
  0x01, 0x52, 0x17, 0x0A,  // 49:338
  0x01, 0x69, 0x1A, 0x0A,  // 50:361
  0x01, 0x83, 0x1A, 0x0A,  // 51:387
  0x01, 0x9D, 0x1A, 0x0A,  // 52:413
  0x01, 0xB7, 0x1A, 0x0A,  // 53:439
  0x01, 0xD1, 0x1A, 0x0A,  // 54:465
  0x01, 0xEB, 0x19, 0x0A,  // 55:491
  0x02, 0x04, 0x1A, 0x0A,  // 56:516
  0x02, 0x1E, 0x1A, 0x0A,  // 57:542
  0x02, 0x38, 0x11, 0x0A,  // 58:568
  0x02, 0x49, 0x11, 0x0A,  // 59:585
  0x02, 0x5A, 0x1A, 0x0A,  // 60:602
  0x02, 0x74, 0x1A, 0x0A,  // 61:628
  0x02, 0x8E, 0x1A, 0x0A,  // 62:654
  0x02, 0xA8, 0x16, 0x0A,  // 63:680
  0x02, 0xBE, 0x1A, 0x0A,  // 64:702
  0x02, 0xD8, 0x1A, 0x0A,  // 65:728
  0x02, 0xF2, 0x1A, 0x0A,  // 66:754
  0x03, 0x0C, 0x1A, 0x0A,  // 67:780
  0x03, 0x26, 0x1A, 0x0A,  // 68:806
  0x03, 0x40, 0x1A, 0x0A,  // 69:832
  0x03, 0x5A, 0x19, 0x0A,  // 70:858
  0x03, 0x73, 0x1A, 0x0A,  // 71:883
  0x03, 0x8D, 0x1A, 0x0A,  // 72:909
  0x03, 0xA7, 0x14, 0x0A,  // 73:935
  0x03, 0xBB, 0x17, 0x0A,  // 74:955
  0x03, 0xD2, 0x1A, 0x0A,  // 75:978
  0x03, 0xEC, 0x1A, 0x0A,  // 76:1004
  0x04, 0x06, 0x1A, 0x0A,  // 77:1030
  0x04, 0x20, 0x1A, 0x0A,  // 78:1056
  0x04, 0x3A, 0x1A, 0x0A,  // 79:1082
  0x04, 0x54, 0x19, 0x0A,  // 80:1108
  0x04, 0x6D, 0x1A, 0x0A,  // 81:1133
  0x04, 0x87, 0x1D, 0x0A,  // 82:1159
  0x04, 0xA4, 0x1A, 0x0A,  // 83:1188
  0x04, 0xBE, 0x19, 0x0A,  // 84:1214
  0x04, 0xD7, 0x1A, 0x0A,  // 85:1239
  0x04, 0xF1, 0x19, 0x0A,  // 86:1265
  0x05, 0x0A, 0x1C, 0x0A,  // 87:1290
  0x05, 0x26, 0x1A, 0x0A,  // 88:1318
  0x05, 0x40, 0x19, 0x0A,  // 89:1344
  0x05, 0x59, 0x1A, 0x0A,  // 90:1369
  0x05, 0x73, 0x15, 0x0A,  // 91:1395
  0x05, 0x88, 0x17, 0x0A,  // 92:1416
  0x05, 0x9F, 0x12, 0x0A,  // 93:1439
  0x05, 0xB1, 0x1C, 0x0A,  // 94:1457
  0x05, 0xCD, 0x1E, 0x0A,  // 95:1485
  0x05, 0xEB, 0x10, 0x0A,  // 96:1515
  0x05, 0xFB, 0x1A, 0x0A,  // 97:1531
  0x06, 0x15, 0x1A, 0x0A,  // 98:1557
  0x06, 0x2F, 0x17, 0x0A,  // 99:1583
  0x06, 0x46, 0x1A, 0x0A,  // 100:1606
  0x06, 0x60, 0x1A, 0x0A,  // 101:1632
  0x06, 0x7A, 0x16, 0x0A,  // 102:1658
  0x06, 0x90, 0x1A, 0x0A,  // 103:1680
  0x06, 0xAA, 0x1A, 0x0A,  // 104:1706
  0x06, 0xC4, 0x17, 0x0A,  // 105:1732
  0x06, 0xDB, 0x12, 0x0A,  // 106:1755
  0x06, 0xED, 0x1A, 0x0A,  // 107:1773
  0x07, 0x07, 0x17, 0x0A,  // 108:1799
  0x07, 0x1E, 0x17, 0x0A,  // 109:1822
  0x07, 0x35, 0x1A, 0x0A,  // 110:1845
  0x07, 0x4F, 0x1A, 0x0A,  // 111:1871
  0x07, 0x69, 0x1A, 0x0A,  // 112:1897
  0x07, 0x83, 0x1B, 0x0A,  // 113:1923
  0x07, 0x9E, 0x19, 0x0A,  // 114:1950
  0x07, 0xB7, 0x1A, 0x0A,  // 115:1975
  0x07, 0xD1, 0x14, 0x0A,  // 116:2001
  0x07, 0xE5, 0x1A, 0x0A,  // 117:2021
  0x07, 0xFF, 0x19, 0x0A,  // 118:2047
  0x08, 0x18, 0x1C, 0x0A,  // 119:2072
  0x08, 0x34, 0x1A, 0x0A,  // 120:2100
  0x08, 0x4E, 0x19, 0x0A,  // 121:2126
  0x08, 0x67, 0x1A, 0x0A,  // 122:2151
  0x08, 0x81, 0x15, 0x0A,  // 123:2177
  0x08, 0x96, 0x0F, 0x0A,  // 124:2198
  0x08, 0xA5, 0x14, 0x0A,  // 125:2213
  0x08, 0xB9, 0x1A, 0x0A,  // 126:2233
  0x08, 0xD3, 0x1B, 0x0A,  // 127:2259
  0x08, 0xEE, 0x1B, 0x0A,  // 128:2286
  0x09, 0x09, 0x1B, 0x0A,  // 129:2313
  0x09, 0x24, 0x1B, 0x0A,  // 130:2340
  0x09, 0x3F, 0x1B, 0x0A,  // 131:2367
  0x09, 0x5A, 0x1B, 0x0A,  // 132:2394
  0x09, 0x75, 0x1B, 0x0A,  // 133:2421
  0x09, 0x90, 0x1B, 0x0A,  // 134:2448
  0x09, 0xAB, 0x1B, 0x0A,  // 135:2475
  0x09, 0xC6, 0x1B, 0x0A,  // 136:2502
  0x09, 0xE1, 0x1B, 0x0A,  // 137:2529
  0x09, 0xFC, 0x1B, 0x0A,  // 138:2556
  0x0A, 0x17, 0x1B, 0x0A,  // 139:2583
  0x0A, 0x32, 0x1B, 0x0A,  // 140:2610
  0x0A, 0x4D, 0x1B, 0x0A,  // 141:2637
  0x0A, 0x68, 0x1B, 0x0A,  // 142:2664
  0x0A, 0x83, 0x1B, 0x0A,  // 143:2691
  0x0A, 0x9E, 0x1B, 0x0A,  // 144:2718
  0x0A, 0xB9, 0x1B, 0x0A,  // 145:2745
  0x0A, 0xD4, 0x1B, 0x0A,  // 146:2772
  0x0A, 0xEF, 0x1B, 0x0A,  // 147:2799
  0x0B, 0x0A, 0x1B, 0x0A,  // 148:2826
  0x0B, 0x25, 0x1B, 0x0A,  // 149:2853
  0x0B, 0x40, 0x1B, 0x0A,  // 150:2880
  0x0B, 0x5B, 0x1B, 0x0A,  // 151:2907
  0x0B, 0x76, 0x1B, 0x0A,  // 152:2934
  0x0B, 0x91, 0x1B, 0x0A,  // 153:2961
  0x0B, 0xAC, 0x1B, 0x0A,  // 154:2988
  0x0B, 0xC7, 0x1B, 0x0A,  // 155:3015
  0x0B, 0xE2, 0x1B, 0x0A,  // 156:3042
  0x0B, 0xFD, 0x1B, 0x0A,  // 157:3069
  0x0C, 0x18, 0x1B, 0x0A,  // 158:3096
  0x0C, 0x33, 0x1B, 0x0A,  // 159:3123
  0xFF, 0xFF, 0x00, 0x0A,  // 160:65535
  0x0C, 0x4E, 0x0F, 0x0A,  // 161:3150
  0x0C, 0x5D, 0x17, 0x0A,  // 162:3165
  0x0C, 0x74, 0x17, 0x0A,  // 163:3188
  0x0C, 0x8B, 0x17, 0x0A,  // 164:3211
  0x0C, 0xA2, 0x19, 0x0A,  // 165:3234
  0x0C, 0xBB, 0x0F, 0x0A,  // 166:3259
  0x0C, 0xCA, 0x1A, 0x0A,  // 167:3274
  0x0C, 0xE4, 0x16, 0x0A,  // 168:3300
  0x0C, 0xFA, 0x1D, 0x0A,  // 169:3322
  0x0D, 0x17, 0x17, 0x0A,  // 170:3351
  0x0D, 0x2E, 0x1A, 0x0A,  // 171:3374
  0x0D, 0x48, 0x1A, 0x0A,  // 172:3400
  0x0D, 0x62, 0x14, 0x0A,  // 173:3426
  0x0D, 0x76, 0x1D, 0x0A,  // 174:3446
  0x0D, 0x93, 0x13, 0x0A,  // 175:3475
  0x0D, 0xA6, 0x13, 0x0A,  // 176:3494
  0x0D, 0xB9, 0x17, 0x0A,  // 177:3513
  0x0D, 0xD0, 0x14, 0x0A,  // 178:3536
  0x0D, 0xE4, 0x17, 0x0A,  // 179:3556
  0x0D, 0xFB, 0x16, 0x0A,  // 180:3579
  0x0E, 0x11, 0x1D, 0x0A,  // 181:3601
  0x0E, 0x2E, 0x1B, 0x0A,  // 182:3630
  0x0E, 0x49, 0x11, 0x0A,  // 183:3657
  0x0E, 0x5A, 0x12, 0x0A,  // 184:3674
  0x0E, 0x6C, 0x14, 0x0A,  // 185:3692
  0x0E, 0x80, 0x17, 0x0A,  // 186:3712
  0x0E, 0x97, 0x1D, 0x0A,  // 187:3735
  0x0E, 0xB4, 0x1A, 0x0A,  // 188:3764
  0x0E, 0xCE, 0x1B, 0x0A,  // 189:3790
  0x0E, 0xE9, 0x1A, 0x0A,  // 190:3817
  0x0F, 0x03, 0x15, 0x0A,  // 191:3843
  0x0F, 0x18, 0x1A, 0x0A,  // 192:3864
  0x0F, 0x32, 0x1A, 0x0A,  // 193:3890
  0x0F, 0x4C, 0x1A, 0x0A,  // 194:3916
  0x0F, 0x66, 0x1A, 0x0A,  // 195:3942
  0x0F, 0x80, 0x1A, 0x0A,  // 196:3968
  0x0F, 0x9A, 0x1A, 0x0A,  // 197:3994
  0x0F, 0xB4, 0x1A, 0x0A,  // 198:4020
  0x0F, 0xCE, 0x1A, 0x0A,  // 199:4046
  0x0F, 0xE8, 0x1A, 0x0A,  // 200:4072
  0x10, 0x02, 0x1A, 0x0A,  // 201:4098
  0x10, 0x1C, 0x1A, 0x0A,  // 202:4124
  0x10, 0x36, 0x1A, 0x0A,  // 203:4150
  0x10, 0x50, 0x14, 0x0A,  // 204:4176
  0x10, 0x64, 0x14, 0x0A,  // 205:4196
  0x10, 0x78, 0x14, 0x0A,  // 206:4216
  0x10, 0x8C, 0x14, 0x0A,  // 207:4236
  0x10, 0xA0, 0x1A, 0x0A,  // 208:4256
  0x10, 0xBA, 0x1A, 0x0A,  // 209:4282
  0x10, 0xD4, 0x1A, 0x0A,  // 210:4308
  0x10, 0xEE, 0x1A, 0x0A,  // 211:4334
  0x11, 0x08, 0x1A, 0x0A,  // 212:4360
  0x11, 0x22, 0x1A, 0x0A,  // 213:4386
  0x11, 0x3C, 0x1A, 0x0A,  // 214:4412
  0x11, 0x56, 0x17, 0x0A,  // 215:4438
  0x11, 0x6D, 0x1A, 0x0A,  // 216:4461
  0x11, 0x87, 0x1A, 0x0A,  // 217:4487
  0x11, 0xA1, 0x1A, 0x0A,  // 218:4513
  0x11, 0xBB, 0x1A, 0x0A,  // 219:4539
  0x11, 0xD5, 0x1A, 0x0A,  // 220:4565
  0x11, 0xEF, 0x19, 0x0A,  // 221:4591
  0x12, 0x08, 0x1A, 0x0A,  // 222:4616
  0x12, 0x22, 0x1A, 0x0A,  // 223:4642
  0x12, 0x3C, 0x1A, 0x0A,  // 224:4668
  0x12, 0x56, 0x1A, 0x0A,  // 225:4694
  0x12, 0x70, 0x1A, 0x0A,  // 226:4720
  0x12, 0x8A, 0x1A, 0x0A,  // 227:4746
  0x12, 0xA4, 0x1A, 0x0A,  // 228:4772
  0x12, 0xBE, 0x1A, 0x0A,  // 229:4798
  0x12, 0xD8, 0x1A, 0x0A,  // 230:4824
  0x12, 0xF2, 0x17, 0x0A,  // 231:4850
  0x13, 0x09, 0x1A, 0x0A,  // 232:4873
  0x13, 0x23, 0x1A, 0x0A,  // 233:4899
  0x13, 0x3D, 0x1A, 0x0A,  // 234:4925
  0x13, 0x57, 0x1A, 0x0A,  // 235:4951
  0x13, 0x71, 0x17, 0x0A,  // 236:4977
  0x13, 0x88, 0x17, 0x0A,  // 237:5000
  0x13, 0x9F, 0x17, 0x0A,  // 238:5023
  0x13, 0xB6, 0x17, 0x0A,  // 239:5046
  0x13, 0xCD, 0x1A, 0x0A,  // 240:5069
  0x13, 0xE7, 0x1A, 0x0A,  // 241:5095
  0x14, 0x01, 0x1A, 0x0A,  // 242:5121
  0x14, 0x1B, 0x1A, 0x0A,  // 243:5147
  0x14, 0x35, 0x1A, 0x0A,  // 244:5173
  0x14, 0x4F, 0x1A, 0x0A,  // 245:5199
  0x14, 0x69, 0x1A, 0x0A,  // 246:5225
  0x14, 0x83, 0x1A, 0x0A,  // 247:5251
  0x14, 0x9D, 0x1A, 0x0A,  // 248:5277
  0x14, 0xB7, 0x1A, 0x0A,  // 249:5303
  0x14, 0xD1, 0x1A, 0x0A,  // 250:5329
  0x14, 0xEB, 0x1A, 0x0A,  // 251:5355
  0x15, 0x05, 0x1A, 0x0A,  // 252:5381
  0x15, 0x1F, 0x19, 0x0A,  // 253:5407
  0x15, 0x38, 0x1A, 0x0A,  // 254:5432
  0x15, 0x52, 0x19, 0x0A,  // 255:5458

  // Font Data:
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x67,  // 33
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78, // 34
  0x00,0x08,0x00,0x80,0x48,0x00,0x80,0x78,0x00,0x80,0x0F,0x00,0xF0,0x08,0x00,0x80,0x78,0x00,0x80,0x1F,0x00,0xF0,0x09,0x00,0x90,0x08,0x00,0x80,  // 35
  0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x21,0x00,0x60,0x42,0x00,0x20,0x42,0x00,0xF8,0xFF,0x01,0x20,0x44,0x00,0x20,0x44,0x00,0x40,0x38,  // 36
  0x70,0x00,0x00,0x88,0x04,0x00,0x88,0x04,0x00,0x88,0x02,0x00,0x70,0x3A,0x00,0x00,0x45,0x00,0x00,0x45,0x00,0x80,0x44,0x00,0x00,0x38,  // 37
  0x00,0x00,0x00,0x00,0x1E,0x00,0x70,0x23,0x00,0x88,0x40,0x00,0x08,0x43,0x00,0x08,0x46,0x00,0x08,0x78,0x00,0x00,0x30,0x00,0x00,0x4E,  // 38
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78, // 39
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x1F,0x00,0x70,0xE0,0x00,0x18,0x80,0x01,0x08,0x00,0x01, // 40
  0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x01,0x18,0x80,0x01,0x70,0xE0,0x00,0x80,0x1F, // 41
  0x00,0x00,0x00,0x20,0x01,0x00,0x40,0x01,0x00,0xC0,0x00,0x00,0xF8,0x07,0x00,0xC0,0x00,0x00,0x40,0x01,0x00,0x20,0x01, // 42
  0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x80,0x3F,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04, // 43
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0xE0,0x01,0x00,0xE0, // 44
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,  // 45
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x60, // 46
  0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x60,0x00,0x00,0x18,0x00,0x00,0x07,0x00,0xC0,0x00,0x00,0x30,0x00,0x00,0x08,  // 47
  0x00,0x00,0x00,0xC0,0x0F,0x00,0x30,0x30,0x00,0x08,0x40,0x00,0x08,0x43,0x00,0x08,0x43,0x00,0x08,0x40,0x00,0x30,0x30,0x00,0xC0,0x0F,  // 48
  0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x18,0x40,0x00,0x08,0x40,0x00,0xF8,0x7F,0x00,0x00,0x40,0x00,0x00,0x40, // 49
  0x00,0x00,0x00,0x30,0x40,0x00,0x18,0x60,0x00,0x08,0x70,0x00,0x08,0x48,0x00,0x08,0x44,0x00,0x08,0x42,0x00,0x10,0x41,0x00,0xE0,0x40,  // 50
  0x00,0x00,0x00,0x10,0x20,0x00,0x08,0x40,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x90,0x22,0x00,0xE0,0x1C,  // 51
  0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x0B,0x00,0xC0,0x08,0x00,0x70,0x08,0x00,0x18,0x08,0x00,0xF8,0x7F,0x00,0x00,0x08,0x00,0x00,0x08,  // 52
  0x00,0x00,0x00,0x00,0x20,0x00,0xF8,0x41,0x00,0x88,0x40,0x00,0x88,0x40,0x00,0x88,0x40,0x00,0x88,0x40,0x00,0x08,0x21,0x00,0x00,0x1E,  // 53
  0x00,0x00,0x00,0xC0,0x1F,0x00,0x30,0x21,0x00,0x98,0x40,0x00,0x88,0x40,0x00,0x88,0x40,0x00,0x88,0x40,0x00,0x10,0x21,0x00,0x00,0x1E,  // 54
  0x00,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x40,0x00,0x08,0x30,0x00,0x08,0x0E,0x00,0x88,0x01,0x00,0x68,0x00,0x00,0x18, // 55
  0x00,0x00,0x00,0xF0,0x1C,0x00,0x90,0x22,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x90,0x22,0x00,0xF0,0x1E,  // 56
  0x00,0x00,0x00,0xE0,0x01,0x00,0x10,0x22,0x00,0x08,0x44,0x00,0x08,0x44,0x00,0x08,0x44,0x00,0x08,0x64,0x00,0x10,0x32,0x00,0xE0,0x0F,  // 57
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x61,0x00,0x80,0x61, // 58
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x80,0xE1,0x01,0x80,0xE1, // 59
  0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x06,0x00,0x00,0x0F,0x00,0x00,0x09,0x00,0x80,0x19,0x00,0x80,0x10,0x00,0x80,0x10,0x00,0xC0,0x30,  // 60
  0x00,0x00,0x00,0x00,0x09,0x00,0x00,0x09,0x00,0x00,0x09,0x00,0x00,0x09,0x00,0x00,0x09,0x00,0x00,0x09,0x00,0x00,0x09,0x00,0x00,0x09,  // 61
  0x00,0x00,0x00,0xC0,0x30,0x00,0x80,0x10,0x00,0x80,0x10,0x00,0x80,0x19,0x00,0x00,0x09,0x00,0x00,0x0F,0x00,0x00,0x06,0x00,0x00,0x06,  // 62
  0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x08,0x00,0x00,0x08,0x6E,0x00,0x08,0x01,0x00,0x98,0x01,0x00,0xF0,  // 63
  0x00,0x00,0x00,0x00,0x3F,0x00,0xC0,0xC0,0x00,0x20,0x00,0x01,0x10,0x3F,0x02,0x90,0x61,0x02,0x90,0x40,0x02,0xB0,0x61,0x02,0xE0,0x7F,  // 64
  0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x1E,0x00,0xE0,0x09,0x00,0x38,0x08,0x00,0x38,0x08,0x00,0xE0,0x09,0x00,0x00,0x1E,0x00,0x00,0x60,  // 65
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x90,0x22,0x00,0xF0,0x3E,  // 66
  0x00,0x00,0x00,0xC0,0x0F,0x00,0x30,0x30,0x00,0x10,0x20,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x18,0x60,0x00,0x10,0x20,  // 67
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x10,0x20,0x00,0x30,0x30,0x00,0xC0,0x0F,  // 68
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,  // 69
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08,0x01,0x00,0x08, // 70
  0x00,0x00,0x00,0xC0,0x0F,0x00,0x30,0x30,0x00,0x10,0x20,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x08,0x42,0x00,0x18,0x42,0x00,0x10,0x3E,  // 71
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0xF8,0x7F,  // 72
  0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0xF8,0x7F,0x00,0x08,0x40,0x00,0x08,0x40,  // 73
  0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x60,0x00,0x00,0x40,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x08,0x20,0x00,0xF8,0x1F, // 74
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x00,0x01,0x00,0x80,0x00,0x00,0x40,0x03,0x00,0x20,0x04,0x00,0x10,0x18,0x00,0x08,0x20,0x00,0x00,0x40,  // 75
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,  // 76
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x38,0x00,0x00,0xE0,0x00,0x00,0x00,0x07,0x00,0x00,0x07,0x00,0xE0,0x00,0x00,0x38,0x00,0x00,0xF8,0x7F,  // 77
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x18,0x00,0x00,0x60,0x00,0x00,0x80,0x01,0x00,0x00,0x06,0x00,0x00,0x18,0x00,0x00,0x60,0x00,0xF8,0x7F,  // 78
  0x00,0x00,0x00,0xE0,0x1F,0x00,0x30,0x30,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x30,0x30,0x00,0xE0,0x1F,  // 79
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x10,0x01,0x00,0xE0, // 80
  0x00,0x00,0x00,0xE0,0x1F,0x00,0x30,0x30,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x08,0xC0,0x00,0x30,0xF0,0x01,0xC0,0x1F,  // 81
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0x10,0x05,0x00,0xE0,0x39,0x00,0x00,0x40, // 82
  0x00,0x00,0x00,0xE0,0x30,0x00,0x10,0x61,0x00,0x08,0x41,0x00,0x08,0x43,0x00,0x08,0x42,0x00,0x18,0x42,0x00,0x10,0x22,0x00,0x00,0x3C,  // 83
  0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0xF8,0x7F,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08, // 84
  0x00,0x00,0x00,0xF8,0x1F,0x00,0x00,0x20,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x20,0x00,0xF8,0x1F,  // 85
  0x00,0x00,0x00,0x18,0x00,0x00,0xE0,0x01,0x00,0x00,0x1E,0x00,0x00,0x70,0x00,0x00,0x70,0x00,0x00,0x1E,0x00,0xE0,0x01,0x00,0x18, // 86
  0x38,0x00,0x00,0xC0,0x1F,0x00,0x00,0x60,0x00,0x00,0x1C,0x00,0xC0,0x03,0x00,0xC0,0x03,0x00,0x00,0x1C,0x00,0x00,0x60,0x00,0xC0,0x1F,0x00,0x38,  // 87
  0x00,0x00,0x00,0x08,0x40,0x00,0x30,0x30,0x00,0xC0,0x0C,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0xC0,0x0C,0x00,0x30,0x30,0x00,0x08,0x40,  // 88
  0x08,0x00,0x00,0x10,0x00,0x00,0x60,0x00,0x00,0x80,0x01,0x00,0x00,0x7E,0x00,0x80,0x01,0x00,0x60,0x00,0x00,0x10,0x00,0x00,0x08, // 89
  0x00,0x00,0x00,0x08,0x60,0x00,0x08,0x50,0x00,0x08,0x4C,0x00,0x08,0x46,0x00,0x88,0x41,0x00,0xC8,0x40,0x00,0x28,0x40,0x00,0x18,0x40,  // 90
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0xFF,0x01,0x08,0x00,0x01,0x08,0x00,0x01, // 91
  0x00,0x00,0x00,0x08,0x00,0x00,0x30,0x00,0x00,0xC0,0x00,0x00,0x00,0x07,0x00,0x00,0x18,0x00,0x00,0x60,0x00,0x00,0x80, // 92
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x01,0x08,0x00,0x01,0xF8,0xFF,0x01,  // 93
  0x00,0x00,0x00,0x40,0x00,0x00,0x60,0x00,0x00,0x30,0x00,0x00,0x18,0x00,0x00,0x08,0x00,0x00,0x18,0x00,0x00,0x30,0x00,0x00,0x60,0x00,0x00,0x40,  // 94
  0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,  // 95
  0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x0C,0x00,0x00,0x18,0x00,0x00,0x10,  // 96
  0x00,0x00,0x00,0x00,0x3C,0x00,0x80,0x64,0x00,0x40,0x42,0x00,0x40,0x42,0x00,0x40,0x42,0x00,0x40,0x62,0x00,0xC0,0x32,0x00,0x80,0x7F,  // 97
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x80,0x20,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x80,0x20,0x00,0x00,0x1F,  // 98
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x20,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x80,0x20, // 99
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x20,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x80,0x20,0x00,0xF8,0x7F,  // 100
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x24,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x80,0x44,0x00,0x00,0x27,  // 101
  0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0xF0,0x7F,0x00,0x48,0x00,0x00,0x48,0x00,0x00,0x48,  // 102
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x20,0x01,0x40,0x40,0x02,0x40,0x40,0x02,0x40,0x40,0x02,0x40,0x40,0x02,0x80,0x20,0x01,0xC0,0xFF,  // 103
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x80,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0xC0,0x00,0x00,0x80,0x7F,  // 104
  0x00,0x00,0x00,0x00,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0xD8,0x7F,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40, // 105
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x40,0x00,0x02,0x40,0x00,0x02,0xD8,0xFF,0x01,  // 106
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x00,0x04,0x00,0x00,0x02,0x00,0x00,0x05,0x00,0x80,0x08,0x00,0x40,0x10,0x00,0x00,0x20,0x00,0x00,0x40,  // 107
  0x00,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0xF8,0x3F,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40, // 108
  0x00,0x00,0x00,0xC0,0x7F,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0xC0,0x7F,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x80,0x7F, // 109
  0x00,0x00,0x00,0xC0,0x7F,0x00,0x80,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0xC0,0x00,0x00,0x80,0x7F,  // 110
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x20,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x80,0x20,0x00,0x00,0x1F,  // 111
  0x00,0x00,0x00,0xC0,0xFF,0x03,0x80,0x20,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x80,0x20,0x00,0x00,0x1F,  // 112
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x20,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x80,0x20,0x00,0xC0,0xFF,0x03, // 113
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x7F,0x00,0x80,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x80, // 114
  0x00,0x00,0x00,0x80,0x23,0x00,0x80,0x46,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x40,0x2C,0x00,0x80,0x38,  // 115
  0x00,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0xF0,0x3F,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,  // 116
  0x00,0x00,0x00,0xC0,0x3F,0x00,0x00,0x60,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x20,0x00,0xC0,0x7F,  // 117
  0x00,0x00,0x00,0x40,0x00,0x00,0x80,0x03,0x00,0x00,0x1C,0x00,0x00,0x60,0x00,0x00,0x60,0x00,0x00,0x1C,0x00,0x80,0x03,0x00,0x40, // 118
  0xC0,0x00,0x00,0x00,0x1F,0x00,0x00,0x60,0x00,0x00,0x1C,0x00,0x00,0x03,0x00,0x00,0x07,0x00,0x00,0x18,0x00,0x00,0x60,0x00,0x00,0x1F,0x00,0xC0,  // 119
  0x00,0x00,0x00,0x40,0x40,0x00,0xC0,0x60,0x00,0x00,0x11,0x00,0x00,0x0E,0x00,0x00,0x0E,0x00,0x00,0x11,0x00,0xC0,0x60,0x00,0x40,0x40,  // 120
  0x00,0x00,0x00,0x40,0x00,0x00,0x80,0x03,0x02,0x00,0x0C,0x02,0x00,0x70,0x03,0x00,0xE0,0x00,0x00,0x1C,0x00,0x80,0x03,0x00,0x40, // 121
  0x00,0x00,0x00,0x40,0x60,0x00,0x40,0x50,0x00,0x40,0x48,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x40,0x42,0x00,0x40,0x41,0x00,0xC0,0x40,  // 122
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0xF0,0xFB,0x01,0x08,0x00,0x02,0x08,0x00,0x02, // 123
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0xFF,0x07, // 124
  0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x02,0x08,0x00,0x02,0xF0,0xFB,0x01,0x00,0x04,0x00,0x00,0x04,  // 125
  0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x02,  // 126
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 127
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 128
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 129
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 130
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 131
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 132
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 133
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 134
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 135
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 136
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 137
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 138
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 139
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 140
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 141
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 142
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 143
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 144
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 145
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 146
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 147
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 148
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 149
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 150
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 151
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 152
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 153
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 154
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 155
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 156
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 157
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 158
  0x00,0x00,0x00,0xF0,0xFF,0x03,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0x10,0x00,0x02,0xF0,0xFF,0x03, // 159
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xFC,0x03, // 161
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x30,0x00,0x40,0x60,0x00,0x40,0x40,0x00,0xF0,0xFF,0x01,0x40,0x40,0x00,0x80,0x20, // 162
  0x00,0x00,0x00,0x00,0x42,0x00,0x00,0x42,0x00,0xF0,0x7F,0x00,0x18,0x42,0x00,0x08,0x42,0x00,0x08,0x42,0x00,0x10,0x40, // 163
  0x00,0x00,0x00,0x40,0x10,0x00,0x80,0x0F,0x00,0x80,0x08,0x00,0x80,0x08,0x00,0x80,0x08,0x00,0x80,0x0F,0x00,0x40,0x10, // 164
  0x08,0x00,0x00,0x30,0x05,0x00,0x60,0x05,0x00,0x80,0x05,0x00,0x00,0x7E,0x00,0x80,0x05,0x00,0x60,0x05,0x00,0x30,0x05,0x00,0x08, // 165
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF3,0x03, // 166
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0xF0,0x0C,0x01,0xC8,0x18,0x01,0x88,0x10,0x01,0x88,0x31,0x01,0x08,0xF1,0x00,0x00,0x0E,  // 167
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x18,  // 168
  0x80,0x07,0x00,0x60,0x18,0x00,0xA0,0x17,0x00,0xD0,0x2C,0x00,0x50,0x28,0x00,0x50,0x28,0x00,0x50,0x28,0x00,0x20,0x10,0x00,0x60,0x18,0x00,0x80,0x07, // 169
  0x00,0x00,0x00,0x00,0x00,0x00,0xC8,0x04,0x00,0x28,0x05,0x00,0x28,0x05,0x00,0x28,0x05,0x00,0xA8,0x04,0x00,0xF0,0x05, // 170
  0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x0A,0x00,0x00,0x1B,0x00,0x80,0x31,0x00,0x00,0x04,0x00,0x00,0x0A,0x00,0x00,0x1B,0x00,0x80,0x31,  // 171
  0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x0F,  // 172
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,  // 173
  0x80,0x07,0x00,0x60,0x18,0x00,0x20,0x10,0x00,0xD0,0x2F,0x00,0x50,0x21,0x00,0x50,0x23,0x00,0xD0,0x2C,0x00,0x20,0x10,0x00,0x60,0x18,0x00,0x80,0x07, // 174
  0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08,0x00,0x00,0x08, // 175
  0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x00,0x00,0x88,0x00,0x00,0x88,0x00,0x00,0x88,0x00,0x00,0x70, // 176
  0x00,0x00,0x00,0x00,0x41,0x00,0x00,0x41,0x00,0x00,0x41,0x00,0xE0,0x4F,0x00,0x00,0x41,0x00,0x00,0x41,0x00,0x00,0x41, // 177
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x03,0x00,0x88,0x03,0x00,0xC8,0x02,0x00,0x70,0x02,  // 178
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x02,0x00,0x48,0x02,0x00,0x48,0x02,0x00,0x48,0x02,0x00,0xB0,0x01, // 179
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x18,0x00,0x00,0x0C,0x00,0x00,0x04,  // 180
  0x00,0x00,0x00,0xC0,0xFF,0x03,0x00,0x20,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x00,0x20,0x00,0xC0,0x7F,0x00,0x00,0x40, // 181
  0x00,0x00,0x00,0xE0,0x00,0x00,0xF0,0x01,0x00,0xF8,0x03,0x00,0xF8,0x03,0x00,0xF8,0xFF,0x01,0x08,0x00,0x00,0x08,0x00,0x00,0xF8,0xFF,0x01, // 182
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x03, // 183
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x02,0x00,0x80,0x03,  // 184
  0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x02,0x00,0x08,0x02,0x00,0xF8,0x03,0x00,0x00,0x02,0x00,0x00,0x02,  // 185
  0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x04,0x00,0x98,0x05,0x00,0x08,0x05,0x00,0x08,0x05,0x00,0x98,0x05,0x00,0xF0,0x04, // 186
  0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x31,0x00,0x00,0x1B,0x00,0x00,0x0A,0x00,0x00,0x04,0x00,0x80,0x31,0x00,0x00,0x1B,0x00,0x00,0x0A,0x00,0x00,0x04, // 187
  0x04,0x05,0x00,0x04,0x05,0x00,0xFC,0x03,0x00,0x00,0xC3,0x00,0x00,0xB3,0x00,0x00,0x8E,0x00,0x00,0xFE,0x01,0x00,0x81,0x00,0x00,0x01,  // 188
  0x04,0x05,0x00,0x04,0x05,0x00,0xFC,0x03,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0x00,0x86,0x01,0x00,0xC6,0x01,0x00,0x65,0x01,0x00,0x39,0x01, // 189
  0x00,0x04,0x00,0x04,0x05,0x00,0x24,0x03,0x00,0x24,0xC3,0x00,0x24,0xB3,0x00,0xD8,0x8E,0x00,0x00,0xFE,0x01,0x00,0x81,0x00,0x00,0x01,  // 190
  0x00,0x00,0x00,0x00,0xE0,0x01,0x00,0x30,0x03,0x00,0x10,0x02,0xC0,0x0E,0x02,0x00,0x00,0x02,0x00,0x00,0x01, // 191
  0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x1E,0x00,0xE1,0x09,0x00,0x3B,0x08,0x00,0x3A,0x08,0x00,0xE0,0x09,0x00,0x00,0x1E,0x00,0x00,0x60,  // 192
  0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x1E,0x00,0xE0,0x09,0x00,0x3A,0x08,0x00,0x3B,0x08,0x00,0xE1,0x09,0x00,0x00,0x1E,0x00,0x00,0x60,  // 193
  0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x1E,0x00,0xE2,0x09,0x00,0x39,0x08,0x00,0x39,0x08,0x00,0xE2,0x09,0x00,0x00,0x1E,0x00,0x00,0x60,  // 194
  0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x1E,0x00,0xE3,0x09,0x00,0x39,0x08,0x00,0x3A,0x08,0x00,0xE3,0x09,0x00,0x00,0x1E,0x00,0x00,0x60,  // 195
  0x00,0x00,0x00,0x00,0x60,0x00,0x03,0x1E,0x00,0xE3,0x09,0x00,0x38,0x08,0x00,0x38,0x08,0x00,0xE3,0x09,0x00,0x03,0x1E,0x00,0x00,0x60,  // 196
  0x00,0x00,0x00,0x00,0x60,0x00,0x00,0x1E,0x00,0xC6,0x09,0x00,0x39,0x08,0x00,0x39,0x08,0x00,0xC6,0x09,0x00,0x00,0x1E,0x00,0x00,0x60,  // 197
  0x00,0x60,0x00,0x00,0x1E,0x00,0xC0,0x0B,0x00,0x38,0x08,0x00,0x08,0x08,0x00,0xF8,0x7F,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,  // 198
  0x00,0x00,0x00,0xC0,0x0F,0x00,0x30,0x30,0x00,0x10,0x20,0x00,0x08,0x40,0x02,0x08,0x40,0x02,0x08,0xC0,0x03,0x18,0x60,0x00,0x10,0x20,  // 199
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x08,0x41,0x00,0x09,0x41,0x00,0x0B,0x41,0x00,0x0A,0x41,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x41,  // 200
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x0A,0x41,0x00,0x0B,0x41,0x00,0x09,0x41,0x00,0x08,0x41,0x00,0x08,0x41,  // 201
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x0A,0x41,0x00,0x0B,0x41,0x00,0x09,0x41,0x00,0x0B,0x41,0x00,0x0A,0x41,0x00,0x08,0x41,0x00,0x08,0x41,  // 202
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x08,0x41,0x00,0x0B,0x41,0x00,0x0B,0x41,0x00,0x08,0x41,0x00,0x0B,0x41,0x00,0x0B,0x41,0x00,0x08,0x41,  // 203
  0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x40,0x00,0x09,0x40,0x00,0xFB,0x7F,0x00,0x0A,0x40,0x00,0x08,0x40,  // 204
  0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0xFA,0x7F,0x00,0x0B,0x40,0x00,0x09,0x40,  // 205
  0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x40,0x00,0x0B,0x40,0x00,0xF9,0x7F,0x00,0x0B,0x40,0x00,0x0A,0x40,  // 206
  0x00,0x00,0x00,0x00,0x00,0x00,0x0B,0x40,0x00,0x0B,0x40,0x00,0xF8,0x7F,0x00,0x0B,0x40,0x00,0x0B,0x40,  // 207
  0x00,0x01,0x00,0xF8,0x7F,0x00,0x08,0x41,0x00,0x08,0x41,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x10,0x20,0x00,0x30,0x30,0x00,0xC0,0x0F,  // 208
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x18,0x00,0x00,0x63,0x00,0x00,0x81,0x01,0x00,0x02,0x06,0x00,0x03,0x18,0x00,0x00,0x60,0x00,0xF8,0x7F,  // 209
  0x00,0x00,0x00,0xE0,0x1F,0x00,0x30,0x30,0x00,0x09,0x40,0x00,0x0B,0x40,0x00,0x0A,0x40,0x00,0x08,0x40,0x00,0x30,0x30,0x00,0xE0,0x1F,  // 210
  0x00,0x00,0x00,0xE0,0x1F,0x00,0x30,0x30,0x00,0x08,0x40,0x00,0x0A,0x40,0x00,0x0B,0x40,0x00,0x09,0x40,0x00,0x30,0x30,0x00,0xE0,0x1F,  // 211
  0x00,0x00,0x00,0xE0,0x1F,0x00,0x30,0x30,0x00,0x0A,0x40,0x00,0x09,0x40,0x00,0x09,0x40,0x00,0x0A,0x40,0x00,0x30,0x30,0x00,0xE0,0x1F,  // 212
  0x00,0x00,0x00,0xE0,0x1F,0x00,0x30,0x30,0x00,0x0B,0x40,0x00,0x09,0x40,0x00,0x0A,0x40,0x00,0x0B,0x40,0x00,0x30,0x30,0x00,0xE0,0x1F,  // 213
  0x00,0x00,0x00,0xE0,0x1F,0x00,0x33,0x30,0x00,0x0B,0x40,0x00,0x08,0x40,0x00,0x08,0x40,0x00,0x0B,0x40,0x00,0x33,0x30,0x00,0xE0,0x1F,  // 214
  0x00,0x00,0x00,0x40,0x10,0x00,0x80,0x08,0x00,0x00,0x05,0x00,0x00,0x02,0x00,0x00,0x05,0x00,0x80,0x08,0x00,0x40,0x10, // 215
  0x00,0x40,0x00,0xC0,0x3F,0x00,0x30,0x78,0x00,0x08,0x44,0x00,0x08,0x42,0x00,0x88,0x41,0x00,0xC8,0x40,0x00,0x78,0x30,0x00,0xF8,0x0F,  // 216
  0x00,0x00,0x00,0xF8,0x1F,0x00,0x00,0x20,0x00,0x01,0x40,0x00,0x03,0x40,0x00,0x02,0x40,0x00,0x00,0x40,0x00,0x00,0x20,0x00,0xF8,0x1F,  // 217
  0x00,0x00,0x00,0xF8,0x1F,0x00,0x00,0x20,0x00,0x00,0x40,0x00,0x02,0x40,0x00,0x03,0x40,0x00,0x01,0x40,0x00,0x00,0x20,0x00,0xF8,0x1F,  // 218
  0x00,0x00,0x00,0xF8,0x1F,0x00,0x00,0x20,0x00,0x02,0x40,0x00,0x01,0x40,0x00,0x01,0x40,0x00,0x02,0x40,0x00,0x00,0x20,0x00,0xF8,0x1F,  // 219
  0x00,0x00,0x00,0xF8,0x1F,0x00,0x03,0x20,0x00,0x03,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x03,0x40,0x00,0x03,0x20,0x00,0xF8,0x1F,  // 220
  0x08,0x00,0x00,0x10,0x00,0x00,0x60,0x00,0x00,0x80,0x01,0x00,0x02,0x7E,0x00,0x83,0x01,0x00,0x61,0x00,0x00,0x10,0x00,0x00,0x08, // 221
  0x00,0x00,0x00,0xF8,0x7F,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x20,0x08,0x00,0x40,0x04,0x00,0xC0,0x07,  // 222
  0x00,0x00,0x00,0xF0,0x7F,0x00,0x18,0x00,0x00,0x08,0x40,0x00,0x88,0x43,0x00,0x48,0x46,0x00,0x58,0x44,0x00,0x30,0x4C,0x00,0x00,0x38,  // 223
  0x00,0x00,0x00,0x00,0x3C,0x00,0x84,0x64,0x00,0x4C,0x42,0x00,0x58,0x42,0x00,0x50,0x42,0x00,0x40,0x62,0x00,0xC0,0x32,0x00,0x80,0x7F,  // 224
  0x00,0x00,0x00,0x00,0x3C,0x00,0x80,0x64,0x00,0x40,0x42,0x00,0x50,0x42,0x00,0x58,0x42,0x00,0x4C,0x62,0x00,0xC4,0x32,0x00,0x80,0x7F,  // 225
  0x00,0x00,0x00,0x00,0x3C,0x00,0x90,0x64,0x00,0x48,0x42,0x00,0x44,0x42,0x00,0x48,0x42,0x00,0x50,0x62,0x00,0xC0,0x32,0x00,0x80,0x7F,  // 226
  0x00,0x00,0x00,0x00,0x3C,0x00,0x98,0x64,0x00,0x48,0x42,0x00,0x58,0x42,0x00,0x50,0x42,0x00,0x58,0x62,0x00,0xC0,0x32,0x00,0x80,0x7F,  // 227
  0x00,0x00,0x00,0x00,0x3C,0x00,0x80,0x64,0x00,0x58,0x42,0x00,0x58,0x42,0x00,0x40,0x42,0x00,0x58,0x62,0x00,0xD8,0x32,0x00,0x80,0x7F,  // 228
  0x00,0x00,0x00,0x00,0x3C,0x00,0x80,0x64,0x00,0x4C,0x42,0x00,0x52,0x42,0x00,0x52,0x42,0x00,0x4C,0x62,0x00,0xC0,0x32,0x00,0x80,0x7F,  // 229
  0x00,0x00,0x00,0x00,0x38,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x80,0x3F,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x40,0x44,0x00,0x80,0x27,  // 230
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x20,0x00,0x40,0x40,0x00,0x40,0x40,0x02,0x40,0x40,0x02,0x40,0xC0,0x03,0x80,0x20, // 231
  0x00,0x00,0x00,0x00,0x1F,0x00,0x84,0x24,0x00,0x4C,0x44,0x00,0x58,0x44,0x00,0x50,0x44,0x00,0x40,0x44,0x00,0x80,0x44,0x00,0x00,0x27,  // 232
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x24,0x00,0x40,0x44,0x00,0x50,0x44,0x00,0x58,0x44,0x00,0x4C,0x44,0x00,0x84,0x44,0x00,0x00,0x27,  // 233
  0x00,0x00,0x00,0x00,0x1F,0x00,0x90,0x24,0x00,0x48,0x44,0x00,0x44,0x44,0x00,0x48,0x44,0x00,0x50,0x44,0x00,0x80,0x44,0x00,0x00,0x27,  // 234
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x24,0x00,0x58,0x44,0x00,0x58,0x44,0x00,0x40,0x44,0x00,0x58,0x44,0x00,0x98,0x44,0x00,0x00,0x27,  // 235
  0x00,0x00,0x00,0x00,0x40,0x00,0x44,0x40,0x00,0x4C,0x40,0x00,0xD8,0x7F,0x00,0x10,0x40,0x00,0x00,0x40,0x00,0x00,0x40, // 236
  0x00,0x00,0x00,0x00,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0xD0,0x7F,0x00,0x18,0x40,0x00,0x0C,0x40,0x00,0x04,0x40, // 237
  0x00,0x00,0x00,0x00,0x40,0x00,0x50,0x40,0x00,0x48,0x40,0x00,0xC4,0x7F,0x00,0x08,0x40,0x00,0x10,0x40,0x00,0x00,0x40, // 238
  0x00,0x00,0x00,0x00,0x40,0x00,0x58,0x40,0x00,0x58,0x40,0x00,0xC0,0x7F,0x00,0x18,0x40,0x00,0x18,0x40,0x00,0x00,0x40, // 239
  0x00,0x00,0x00,0x00,0x1F,0x00,0xA0,0x20,0x00,0x58,0x40,0x00,0x58,0x40,0x00,0x50,0x40,0x00,0x70,0x40,0x00,0xC8,0x20,0x00,0x00,0x1F,  // 240
  0x00,0x00,0x00,0xC0,0x7F,0x00,0x98,0x00,0x00,0x48,0x00,0x00,0x58,0x00,0x00,0x50,0x00,0x00,0x58,0x00,0x00,0xC0,0x00,0x00,0x80,0x7F,  // 241
  0x00,0x00,0x00,0x00,0x1F,0x00,0x84,0x20,0x00,0x4C,0x40,0x00,0x58,0x40,0x00,0x50,0x40,0x00,0x40,0x40,0x00,0x80,0x20,0x00,0x00,0x1F,  // 242
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x20,0x00,0x40,0x40,0x00,0x50,0x40,0x00,0x58,0x40,0x00,0x4C,0x40,0x00,0x84,0x20,0x00,0x00,0x1F,  // 243
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x20,0x00,0x50,0x40,0x00,0x4C,0x40,0x00,0x4C,0x40,0x00,0x50,0x40,0x00,0x80,0x20,0x00,0x00,0x1F,  // 244
  0x00,0x00,0x00,0x00,0x1F,0x00,0x80,0x20,0x00,0x58,0x40,0x00,0x48,0x40,0x00,0x50,0x40,0x00,0x58,0x40,0x00,0x80,0x20,0x00,0x00,0x1F,  // 245
  0x00,0x00,0x00,0x00,0x1F,0x00,0x98,0x20,0x00,0x58,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x58,0x40,0x00,0x98,0x20,0x00,0x00,0x1F,  // 246
  0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x80,0x35,0x00,0x80,0x35,0x00,0x00,0x04,0x00,0x00,0x04,0x00,0x00,0x04,  // 247
  0x00,0x00,0x00,0x00,0x7F,0x00,0x80,0x70,0x00,0x40,0x58,0x00,0x40,0x4C,0x00,0x40,0x46,0x00,0x40,0x43,0x00,0xC0,0x21,0x00,0xC0,0x1F,  // 248
  0x00,0x00,0x00,0xC0,0x3F,0x00,0x04,0x60,0x00,0x0C,0x40,0x00,0x18,0x40,0x00,0x10,0x40,0x00,0x00,0x40,0x00,0x00,0x20,0x00,0xC0,0x7F,  // 249
  0x00,0x00,0x00,0xC0,0x3F,0x00,0x00,0x60,0x00,0x00,0x40,0x00,0x10,0x40,0x00,0x18,0x40,0x00,0x0C,0x40,0x00,0x04,0x20,0x00,0xC0,0x7F,  // 250
  0x00,0x00,0x00,0xC0,0x3F,0x00,0x00,0x60,0x00,0x10,0x40,0x00,0x0C,0x40,0x00,0x0C,0x40,0x00,0x10,0x40,0x00,0x00,0x20,0x00,0xC0,0x7F,  // 251
  0x00,0x00,0x00,0xC0,0x3F,0x00,0x18,0x60,0x00,0x18,0x40,0x00,0x00,0x40,0x00,0x00,0x40,0x00,0x18,0x40,0x00,0x18,0x20,0x00,0xC0,0x7F,  // 252
  0x00,0x00,0x00,0x40,0x00,0x00,0x80,0x03,0x02,0x00,0x0C,0x02,0x10,0x70,0x03,0x18,0xE0,0x00,0x0C,0x1C,0x00,0x84,0x03,0x00,0x40, // 253
  0x00,0x00,0x00,0xF8,0xFF,0x03,0x80,0x20,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x40,0x40,0x00,0x80,0x20,0x00,0x00,0x1F,  // 254
  0x00,0x00,0x00,0x40,0x00,0x00,0x80,0x03,0x02,0x18,0x0C,0x02,0x18,0x70,0x03,0x00,0xE0,0x00,0x18,0x1C,0x00,0x98,0x03,0x00,0x40  // 255
};
