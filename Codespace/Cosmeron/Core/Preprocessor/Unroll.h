#pragma once

#include "Compiling.inc"
#include "Operations.h"

/* ============================================================
 * PREPROCESSOR - NUMERIC DECREMENT & INCREMENT (0..256)
 * ============================================================ */
#define PP_DEC(x) PP_OP_CAT2(PP_DEC_, x)
#define PP_DEC_0 0
#define PP_DEC_1 0
#define PP_DEC_2 1
#define PP_DEC_3 2
#define PP_DEC_4 3
#define PP_DEC_5 4
#define PP_DEC_6 5
#define PP_DEC_7 6
#define PP_DEC_8 7
#define PP_DEC_9 8
#define PP_DEC_10 9
#define PP_DEC_11 10
#define PP_DEC_12 11
#define PP_DEC_13 12
#define PP_DEC_14 13
#define PP_DEC_15 14
#define PP_DEC_16 15
#define PP_DEC_17 16
#define PP_DEC_18 17
#define PP_DEC_19 18
#define PP_DEC_20 19
#define PP_DEC_21 20
#define PP_DEC_22 21
#define PP_DEC_23 22
#define PP_DEC_24 23
#define PP_DEC_25 24
#define PP_DEC_26 25
#define PP_DEC_27 26
#define PP_DEC_28 27
#define PP_DEC_29 28
#define PP_DEC_30 29
#define PP_DEC_31 30
#define PP_DEC_32 31
#define PP_DEC_33 32
#define PP_DEC_34 33
#define PP_DEC_35 34
#define PP_DEC_36 35
#define PP_DEC_37 36
#define PP_DEC_38 37
#define PP_DEC_39 38
#define PP_DEC_40 39
#define PP_DEC_41 40
#define PP_DEC_42 41
#define PP_DEC_43 42
#define PP_DEC_44 43
#define PP_DEC_45 44
#define PP_DEC_46 45
#define PP_DEC_47 46
#define PP_DEC_48 47
#define PP_DEC_49 48
#define PP_DEC_50 49
#define PP_DEC_51 50
#define PP_DEC_52 51
#define PP_DEC_53 52
#define PP_DEC_54 53
#define PP_DEC_55 54
#define PP_DEC_56 55
#define PP_DEC_57 56
#define PP_DEC_58 57
#define PP_DEC_59 58
#define PP_DEC_60 59
#define PP_DEC_61 60
#define PP_DEC_62 61
#define PP_DEC_63 62
#define PP_DEC_64 63
#define PP_DEC_65 64
#define PP_DEC_66 65
#define PP_DEC_67 66
#define PP_DEC_68 67
#define PP_DEC_69 68
#define PP_DEC_70 69
#define PP_DEC_71 70
#define PP_DEC_72 71
#define PP_DEC_73 72
#define PP_DEC_74 73
#define PP_DEC_75 74
#define PP_DEC_76 75
#define PP_DEC_77 76
#define PP_DEC_78 77
#define PP_DEC_79 78
#define PP_DEC_80 79
#define PP_DEC_81 80
#define PP_DEC_82 81
#define PP_DEC_83 82
#define PP_DEC_84 83
#define PP_DEC_85 84
#define PP_DEC_86 85
#define PP_DEC_87 86
#define PP_DEC_88 87
#define PP_DEC_89 88
#define PP_DEC_90 89
#define PP_DEC_91 90
#define PP_DEC_92 91
#define PP_DEC_93 92
#define PP_DEC_94 93
#define PP_DEC_95 94
#define PP_DEC_96 95
#define PP_DEC_97 96
#define PP_DEC_98 97
#define PP_DEC_99 98
#define PP_DEC_100 99
#define PP_DEC_101 100
#define PP_DEC_102 101
#define PP_DEC_103 102
#define PP_DEC_104 103
#define PP_DEC_105 104
#define PP_DEC_106 105
#define PP_DEC_107 106
#define PP_DEC_108 107
#define PP_DEC_109 108
#define PP_DEC_110 109
#define PP_DEC_111 110
#define PP_DEC_112 111
#define PP_DEC_113 112
#define PP_DEC_114 113
#define PP_DEC_115 114
#define PP_DEC_116 115
#define PP_DEC_117 116
#define PP_DEC_118 117
#define PP_DEC_119 118
#define PP_DEC_120 119
#define PP_DEC_121 120
#define PP_DEC_122 121
#define PP_DEC_123 122
#define PP_DEC_124 123
#define PP_DEC_125 124
#define PP_DEC_126 125
#define PP_DEC_127 126
#define PP_DEC_128 127
#define PP_DEC_129 128
#define PP_DEC_130 129
#define PP_DEC_131 130
#define PP_DEC_132 131
#define PP_DEC_133 132
#define PP_DEC_134 133
#define PP_DEC_135 134
#define PP_DEC_136 135
#define PP_DEC_137 136
#define PP_DEC_138 137
#define PP_DEC_139 138
#define PP_DEC_140 139
#define PP_DEC_141 140
#define PP_DEC_142 141
#define PP_DEC_143 142
#define PP_DEC_144 143
#define PP_DEC_145 144
#define PP_DEC_146 145
#define PP_DEC_147 146
#define PP_DEC_148 147
#define PP_DEC_149 148
#define PP_DEC_150 149
#define PP_DEC_151 150
#define PP_DEC_152 151
#define PP_DEC_153 152
#define PP_DEC_154 153
#define PP_DEC_155 154
#define PP_DEC_156 155
#define PP_DEC_157 156
#define PP_DEC_158 157
#define PP_DEC_159 158
#define PP_DEC_160 159
#define PP_DEC_161 160
#define PP_DEC_162 161
#define PP_DEC_163 162
#define PP_DEC_164 163
#define PP_DEC_165 164
#define PP_DEC_166 165
#define PP_DEC_167 166
#define PP_DEC_168 167
#define PP_DEC_169 168
#define PP_DEC_170 169
#define PP_DEC_171 170
#define PP_DEC_172 171
#define PP_DEC_173 172
#define PP_DEC_174 173
#define PP_DEC_175 174
#define PP_DEC_176 175
#define PP_DEC_177 176
#define PP_DEC_178 177
#define PP_DEC_179 178
#define PP_DEC_180 179
#define PP_DEC_181 180
#define PP_DEC_182 181
#define PP_DEC_183 182
#define PP_DEC_184 183
#define PP_DEC_185 184
#define PP_DEC_186 185
#define PP_DEC_187 186
#define PP_DEC_188 187
#define PP_DEC_189 188
#define PP_DEC_190 189
#define PP_DEC_191 190
#define PP_DEC_192 191
#define PP_DEC_193 192
#define PP_DEC_194 193
#define PP_DEC_195 194
#define PP_DEC_196 195
#define PP_DEC_197 196
#define PP_DEC_198 197
#define PP_DEC_199 198
#define PP_DEC_200 199
#define PP_DEC_201 200
#define PP_DEC_202 201
#define PP_DEC_203 202
#define PP_DEC_204 203
#define PP_DEC_205 204
#define PP_DEC_206 205
#define PP_DEC_207 206
#define PP_DEC_208 207
#define PP_DEC_209 208
#define PP_DEC_210 209
#define PP_DEC_211 210
#define PP_DEC_212 211
#define PP_DEC_213 212
#define PP_DEC_214 213
#define PP_DEC_215 214
#define PP_DEC_216 215
#define PP_DEC_217 216
#define PP_DEC_218 217
#define PP_DEC_219 218
#define PP_DEC_220 219
#define PP_DEC_221 220
#define PP_DEC_222 221
#define PP_DEC_223 222
#define PP_DEC_224 223
#define PP_DEC_225 224
#define PP_DEC_226 225
#define PP_DEC_227 226
#define PP_DEC_228 227
#define PP_DEC_229 228
#define PP_DEC_230 229
#define PP_DEC_231 230
#define PP_DEC_232 231
#define PP_DEC_233 232
#define PP_DEC_234 233
#define PP_DEC_235 234
#define PP_DEC_236 235
#define PP_DEC_237 236
#define PP_DEC_238 237
#define PP_DEC_239 238
#define PP_DEC_240 239
#define PP_DEC_241 240
#define PP_DEC_242 241
#define PP_DEC_243 242
#define PP_DEC_244 243
#define PP_DEC_245 244
#define PP_DEC_246 245
#define PP_DEC_247 246
#define PP_DEC_248 247
#define PP_DEC_249 248
#define PP_DEC_250 249
#define PP_DEC_251 250
#define PP_DEC_252 251
#define PP_DEC_253 252
#define PP_DEC_254 253
#define PP_DEC_255 254
#define PP_DEC_256 255

#define PP_INC(x) PP_OP_CAT2(PP_INC_, x)
#define PP_INC_0 1
#define PP_INC_1 2
#define PP_INC_2 3
#define PP_INC_3 4
#define PP_INC_4 5
#define PP_INC_5 6
#define PP_INC_6 7
#define PP_INC_7 8
#define PP_INC_8 9
#define PP_INC_9 10
#define PP_INC_10 11
#define PP_INC_11 12
#define PP_INC_12 13
#define PP_INC_13 14
#define PP_INC_14 15
#define PP_INC_15 16
#define PP_INC_16 17
#define PP_INC_17 18
#define PP_INC_18 19
#define PP_INC_19 20
#define PP_INC_20 21
#define PP_INC_21 22
#define PP_INC_22 23
#define PP_INC_23 24
#define PP_INC_24 25
#define PP_INC_25 26
#define PP_INC_26 27
#define PP_INC_27 28
#define PP_INC_28 29
#define PP_INC_29 30
#define PP_INC_30 31
#define PP_INC_31 32
#define PP_INC_32 33
#define PP_INC_33 34
#define PP_INC_34 35
#define PP_INC_35 36
#define PP_INC_36 37
#define PP_INC_37 38
#define PP_INC_38 39
#define PP_INC_39 40
#define PP_INC_40 41
#define PP_INC_41 42
#define PP_INC_42 43
#define PP_INC_43 44
#define PP_INC_44 45
#define PP_INC_45 46
#define PP_INC_46 47
#define PP_INC_47 48
#define PP_INC_48 49
#define PP_INC_49 50
#define PP_INC_50 51
#define PP_INC_51 52
#define PP_INC_52 53
#define PP_INC_53 54
#define PP_INC_54 55
#define PP_INC_55 56
#define PP_INC_56 57
#define PP_INC_57 58
#define PP_INC_58 59
#define PP_INC_59 60
#define PP_INC_60 61
#define PP_INC_61 62
#define PP_INC_62 63
#define PP_INC_63 64
#define PP_INC_64 65
#define PP_INC_65 66
#define PP_INC_66 67
#define PP_INC_67 68
#define PP_INC_68 69
#define PP_INC_69 70
#define PP_INC_70 71
#define PP_INC_71 72
#define PP_INC_72 73
#define PP_INC_73 74
#define PP_INC_74 75
#define PP_INC_75 76
#define PP_INC_76 77
#define PP_INC_77 78
#define PP_INC_78 79
#define PP_INC_79 80
#define PP_INC_80 81
#define PP_INC_81 82
#define PP_INC_82 83
#define PP_INC_83 84
#define PP_INC_84 85
#define PP_INC_85 86
#define PP_INC_86 87
#define PP_INC_87 88
#define PP_INC_88 89
#define PP_INC_89 90
#define PP_INC_90 91
#define PP_INC_91 92
#define PP_INC_92 93
#define PP_INC_93 94
#define PP_INC_94 95
#define PP_INC_95 96
#define PP_INC_96 97
#define PP_INC_97 98
#define PP_INC_98 99
#define PP_INC_99 100
#define PP_INC_100 101
#define PP_INC_101 102
#define PP_INC_102 103
#define PP_INC_103 104
#define PP_INC_104 105
#define PP_INC_105 106
#define PP_INC_106 107
#define PP_INC_107 108
#define PP_INC_108 109
#define PP_INC_109 110
#define PP_INC_110 111
#define PP_INC_111 112
#define PP_INC_112 113
#define PP_INC_113 114
#define PP_INC_114 115
#define PP_INC_115 116
#define PP_INC_116 117
#define PP_INC_117 118
#define PP_INC_118 119
#define PP_INC_119 120
#define PP_INC_120 121
#define PP_INC_121 122
#define PP_INC_122 123
#define PP_INC_123 124
#define PP_INC_124 125
#define PP_INC_125 126
#define PP_INC_126 127
#define PP_INC_127 128
#define PP_INC_128 129
#define PP_INC_129 130
#define PP_INC_130 131
#define PP_INC_131 132
#define PP_INC_132 133
#define PP_INC_133 134
#define PP_INC_134 135
#define PP_INC_135 136
#define PP_INC_136 137
#define PP_INC_137 138
#define PP_INC_138 139
#define PP_INC_139 140
#define PP_INC_140 141
#define PP_INC_141 142
#define PP_INC_142 143
#define PP_INC_143 144
#define PP_INC_144 145
#define PP_INC_145 146
#define PP_INC_146 147
#define PP_INC_147 148
#define PP_INC_148 149
#define PP_INC_149 150
#define PP_INC_150 151
#define PP_INC_151 152
#define PP_INC_152 153
#define PP_INC_153 154
#define PP_INC_154 155
#define PP_INC_155 156
#define PP_INC_156 157
#define PP_INC_157 158
#define PP_INC_158 159
#define PP_INC_159 160
#define PP_INC_160 161
#define PP_INC_161 162
#define PP_INC_162 163
#define PP_INC_163 164
#define PP_INC_164 165
#define PP_INC_165 166
#define PP_INC_166 167
#define PP_INC_167 168
#define PP_INC_168 169
#define PP_INC_169 170
#define PP_INC_170 171
#define PP_INC_171 172
#define PP_INC_172 173
#define PP_INC_173 174
#define PP_INC_174 175
#define PP_INC_175 176
#define PP_INC_176 177
#define PP_INC_177 178
#define PP_INC_178 179
#define PP_INC_179 180
#define PP_INC_180 181
#define PP_INC_181 182
#define PP_INC_182 183
#define PP_INC_183 184
#define PP_INC_184 185
#define PP_INC_185 186
#define PP_INC_186 187
#define PP_INC_187 188
#define PP_INC_188 189
#define PP_INC_189 190
#define PP_INC_190 191
#define PP_INC_191 192
#define PP_INC_192 193
#define PP_INC_193 194
#define PP_INC_194 195
#define PP_INC_195 196
#define PP_INC_196 197
#define PP_INC_197 198
#define PP_INC_198 199
#define PP_INC_199 200
#define PP_INC_200 201
#define PP_INC_201 202
#define PP_INC_202 203
#define PP_INC_203 204
#define PP_INC_204 205
#define PP_INC_205 206
#define PP_INC_206 207
#define PP_INC_207 208
#define PP_INC_208 209
#define PP_INC_209 210
#define PP_INC_210 211
#define PP_INC_211 212
#define PP_INC_212 213
#define PP_INC_213 214
#define PP_INC_214 215
#define PP_INC_215 216
#define PP_INC_216 217
#define PP_INC_217 218
#define PP_INC_218 219
#define PP_INC_219 220
#define PP_INC_220 221
#define PP_INC_221 222
#define PP_INC_222 223
#define PP_INC_223 224
#define PP_INC_224 225
#define PP_INC_225 226
#define PP_INC_226 227
#define PP_INC_227 228
#define PP_INC_228 229
#define PP_INC_229 230
#define PP_INC_230 231
#define PP_INC_231 232
#define PP_INC_232 233
#define PP_INC_233 234
#define PP_INC_234 235
#define PP_INC_235 236
#define PP_INC_236 237
#define PP_INC_237 238
#define PP_INC_238 239
#define PP_INC_239 240
#define PP_INC_240 241
#define PP_INC_241 242
#define PP_INC_242 243
#define PP_INC_243 244
#define PP_INC_244 245
#define PP_INC_245 246
#define PP_INC_246 247
#define PP_INC_247 248
#define PP_INC_248 249
#define PP_INC_249 250
#define PP_INC_250 251
#define PP_INC_251 252
#define PP_INC_252 253
#define PP_INC_253 254
#define PP_INC_254 255
#define PP_INC_255 256
#define PP_INC_256 256

#define PP_REPEAT(count, macro) PP_OP_CAT2(PP_REPEAT_, count)(macro)
#define PP_REPEAT_0(macro)
#define PP_REPEAT_1(macro) macro(1) PP_REPEAT_0(macro)
#define PP_REPEAT_2(macro) macro(2) PP_REPEAT_1(macro)
#define PP_REPEAT_3(macro) macro(3) PP_REPEAT_2(macro)
#define PP_REPEAT_4(macro) macro(4) PP_REPEAT_3(macro)
#define PP_REPEAT_5(macro) macro(5) PP_REPEAT_4(macro)
#define PP_REPEAT_6(macro) macro(6) PP_REPEAT_5(macro)
#define PP_REPEAT_7(macro) macro(7) PP_REPEAT_6(macro)
#define PP_REPEAT_8(macro) macro(8) PP_REPEAT_7(macro)
#define PP_REPEAT_9(macro) macro(9) PP_REPEAT_8(macro)
#define PP_REPEAT_10(macro) macro(10) PP_REPEAT_9(macro)
#define PP_REPEAT_11(macro) macro(11) PP_REPEAT_10(macro)
#define PP_REPEAT_12(macro) macro(12) PP_REPEAT_11(macro)
#define PP_REPEAT_13(macro) macro(13) PP_REPEAT_12(macro)
#define PP_REPEAT_14(macro) macro(14) PP_REPEAT_13(macro)
#define PP_REPEAT_15(macro) macro(15) PP_REPEAT_14(macro)
#define PP_REPEAT_16(macro) macro(16) PP_REPEAT_15(macro)
#define PP_REPEAT_17(macro) macro(17) PP_REPEAT_16(macro)
#define PP_REPEAT_18(macro) macro(18) PP_REPEAT_17(macro)
#define PP_REPEAT_19(macro) macro(19) PP_REPEAT_18(macro)
#define PP_REPEAT_20(macro) macro(20) PP_REPEAT_19(macro)
#define PP_REPEAT_21(macro) macro(21) PP_REPEAT_20(macro)
#define PP_REPEAT_22(macro) macro(22) PP_REPEAT_21(macro)
#define PP_REPEAT_23(macro) macro(23) PP_REPEAT_22(macro)
#define PP_REPEAT_24(macro) macro(24) PP_REPEAT_23(macro)
#define PP_REPEAT_25(macro) macro(25) PP_REPEAT_24(macro)
#define PP_REPEAT_26(macro) macro(26) PP_REPEAT_25(macro)
#define PP_REPEAT_27(macro) macro(27) PP_REPEAT_26(macro)
#define PP_REPEAT_28(macro) macro(28) PP_REPEAT_27(macro)
#define PP_REPEAT_29(macro) macro(29) PP_REPEAT_28(macro)
#define PP_REPEAT_30(macro) macro(30) PP_REPEAT_29(macro)
#define PP_REPEAT_31(macro) macro(31) PP_REPEAT_30(macro)
#define PP_REPEAT_32(macro) macro(32) PP_REPEAT_31(macro)
#define PP_REPEAT_33(macro) macro(33) PP_REPEAT_32(macro)
#define PP_REPEAT_34(macro) macro(34) PP_REPEAT_33(macro)
#define PP_REPEAT_35(macro) macro(35) PP_REPEAT_34(macro)
#define PP_REPEAT_36(macro) macro(36) PP_REPEAT_35(macro)
#define PP_REPEAT_37(macro) macro(37) PP_REPEAT_36(macro)
#define PP_REPEAT_38(macro) macro(38) PP_REPEAT_37(macro)
#define PP_REPEAT_39(macro) macro(39) PP_REPEAT_38(macro)
#define PP_REPEAT_40(macro) macro(40) PP_REPEAT_39(macro)
#define PP_REPEAT_41(macro) macro(41) PP_REPEAT_40(macro)
#define PP_REPEAT_42(macro) macro(42) PP_REPEAT_41(macro)
#define PP_REPEAT_43(macro) macro(43) PP_REPEAT_42(macro)
#define PP_REPEAT_44(macro) macro(44) PP_REPEAT_43(macro)
#define PP_REPEAT_45(macro) macro(45) PP_REPEAT_44(macro)
#define PP_REPEAT_46(macro) macro(46) PP_REPEAT_45(macro)
#define PP_REPEAT_47(macro) macro(47) PP_REPEAT_46(macro)
#define PP_REPEAT_48(macro) macro(48) PP_REPEAT_47(macro)
#define PP_REPEAT_49(macro) macro(49) PP_REPEAT_48(macro)
#define PP_REPEAT_50(macro) macro(50) PP_REPEAT_49(macro)
#define PP_REPEAT_51(macro) macro(51) PP_REPEAT_50(macro)
#define PP_REPEAT_52(macro) macro(52) PP_REPEAT_51(macro)
#define PP_REPEAT_53(macro) macro(53) PP_REPEAT_52(macro)
#define PP_REPEAT_54(macro) macro(54) PP_REPEAT_53(macro)
#define PP_REPEAT_55(macro) macro(55) PP_REPEAT_54(macro)
#define PP_REPEAT_56(macro) macro(56) PP_REPEAT_55(macro)
#define PP_REPEAT_57(macro) macro(57) PP_REPEAT_56(macro)
#define PP_REPEAT_58(macro) macro(58) PP_REPEAT_57(macro)
#define PP_REPEAT_59(macro) macro(59) PP_REPEAT_58(macro)
#define PP_REPEAT_60(macro) macro(60) PP_REPEAT_59(macro)
#define PP_REPEAT_61(macro) macro(61) PP_REPEAT_60(macro)
#define PP_REPEAT_62(macro) macro(62) PP_REPEAT_61(macro)
#define PP_REPEAT_63(macro) macro(63) PP_REPEAT_62(macro)
#define PP_REPEAT_64(macro) macro(64) PP_REPEAT_63(macro)
#define PP_REPEAT_65(macro) macro(65) PP_REPEAT_64(macro)
#define PP_REPEAT_66(macro) macro(66) PP_REPEAT_65(macro)
#define PP_REPEAT_67(macro) macro(67) PP_REPEAT_66(macro)
#define PP_REPEAT_68(macro) macro(68) PP_REPEAT_67(macro)
#define PP_REPEAT_69(macro) macro(69) PP_REPEAT_68(macro)
#define PP_REPEAT_70(macro) macro(70) PP_REPEAT_69(macro)
#define PP_REPEAT_71(macro) macro(71) PP_REPEAT_70(macro)
#define PP_REPEAT_72(macro) macro(72) PP_REPEAT_71(macro)
#define PP_REPEAT_73(macro) macro(73) PP_REPEAT_72(macro)
#define PP_REPEAT_74(macro) macro(74) PP_REPEAT_73(macro)
#define PP_REPEAT_75(macro) macro(75) PP_REPEAT_74(macro)
#define PP_REPEAT_76(macro) macro(76) PP_REPEAT_75(macro)
#define PP_REPEAT_77(macro) macro(77) PP_REPEAT_76(macro)
#define PP_REPEAT_78(macro) macro(78) PP_REPEAT_77(macro)
#define PP_REPEAT_79(macro) macro(79) PP_REPEAT_78(macro)
#define PP_REPEAT_80(macro) macro(80) PP_REPEAT_79(macro)
#define PP_REPEAT_81(macro) macro(81) PP_REPEAT_80(macro)
#define PP_REPEAT_82(macro) macro(82) PP_REPEAT_81(macro)
#define PP_REPEAT_83(macro) macro(83) PP_REPEAT_82(macro)
#define PP_REPEAT_84(macro) macro(84) PP_REPEAT_83(macro)
#define PP_REPEAT_85(macro) macro(85) PP_REPEAT_84(macro)
#define PP_REPEAT_86(macro) macro(86) PP_REPEAT_85(macro)
#define PP_REPEAT_87(macro) macro(87) PP_REPEAT_86(macro)
#define PP_REPEAT_88(macro) macro(88) PP_REPEAT_87(macro)
#define PP_REPEAT_89(macro) macro(89) PP_REPEAT_88(macro)
#define PP_REPEAT_90(macro) macro(90) PP_REPEAT_89(macro)
#define PP_REPEAT_91(macro) macro(91) PP_REPEAT_90(macro)
#define PP_REPEAT_92(macro) macro(92) PP_REPEAT_91(macro)
#define PP_REPEAT_93(macro) macro(93) PP_REPEAT_92(macro)
#define PP_REPEAT_94(macro) macro(94) PP_REPEAT_93(macro)
#define PP_REPEAT_95(macro) macro(95) PP_REPEAT_94(macro)
#define PP_REPEAT_96(macro) macro(96) PP_REPEAT_95(macro)
#define PP_REPEAT_97(macro) macro(97) PP_REPEAT_96(macro)
#define PP_REPEAT_98(macro) macro(98) PP_REPEAT_97(macro)
#define PP_REPEAT_99(macro) macro(99) PP_REPEAT_98(macro)
#define PP_REPEAT_100(macro) macro(100) PP_REPEAT_99(macro)
#define PP_REPEAT_101(macro) macro(101) PP_REPEAT_100(macro)
#define PP_REPEAT_102(macro) macro(102) PP_REPEAT_101(macro)
#define PP_REPEAT_103(macro) macro(103) PP_REPEAT_102(macro)
#define PP_REPEAT_104(macro) macro(104) PP_REPEAT_103(macro)
#define PP_REPEAT_105(macro) macro(105) PP_REPEAT_104(macro)
#define PP_REPEAT_106(macro) macro(106) PP_REPEAT_105(macro)
#define PP_REPEAT_107(macro) macro(107) PP_REPEAT_106(macro)
#define PP_REPEAT_108(macro) macro(108) PP_REPEAT_107(macro)
#define PP_REPEAT_109(macro) macro(109) PP_REPEAT_108(macro)
#define PP_REPEAT_110(macro) macro(110) PP_REPEAT_109(macro)
#define PP_REPEAT_111(macro) macro(111) PP_REPEAT_110(macro)
#define PP_REPEAT_112(macro) macro(112) PP_REPEAT_111(macro)
#define PP_REPEAT_113(macro) macro(113) PP_REPEAT_112(macro)
#define PP_REPEAT_114(macro) macro(114) PP_REPEAT_113(macro)
#define PP_REPEAT_115(macro) macro(115) PP_REPEAT_114(macro)
#define PP_REPEAT_116(macro) macro(116) PP_REPEAT_115(macro)
#define PP_REPEAT_117(macro) macro(117) PP_REPEAT_116(macro)
#define PP_REPEAT_118(macro) macro(118) PP_REPEAT_117(macro)
#define PP_REPEAT_119(macro) macro(119) PP_REPEAT_118(macro)
#define PP_REPEAT_120(macro) macro(120) PP_REPEAT_119(macro)
#define PP_REPEAT_121(macro) macro(121) PP_REPEAT_120(macro)
#define PP_REPEAT_122(macro) macro(122) PP_REPEAT_121(macro)
#define PP_REPEAT_123(macro) macro(123) PP_REPEAT_122(macro)
#define PP_REPEAT_124(macro) macro(124) PP_REPEAT_123(macro)
#define PP_REPEAT_125(macro) macro(125) PP_REPEAT_124(macro)
#define PP_REPEAT_126(macro) macro(126) PP_REPEAT_125(macro)
#define PP_REPEAT_127(macro) macro(127) PP_REPEAT_126(macro)
#define PP_REPEAT_128(macro) macro(128) PP_REPEAT_127(macro)
#define PP_REPEAT_129(macro) macro(129) PP_REPEAT_128(macro)
#define PP_REPEAT_130(macro) macro(130) PP_REPEAT_129(macro)
#define PP_REPEAT_131(macro) macro(131) PP_REPEAT_130(macro)
#define PP_REPEAT_132(macro) macro(132) PP_REPEAT_131(macro)
#define PP_REPEAT_133(macro) macro(133) PP_REPEAT_132(macro)
#define PP_REPEAT_134(macro) macro(134) PP_REPEAT_133(macro)
#define PP_REPEAT_135(macro) macro(135) PP_REPEAT_134(macro)
#define PP_REPEAT_136(macro) macro(136) PP_REPEAT_135(macro)
#define PP_REPEAT_137(macro) macro(137) PP_REPEAT_136(macro)
#define PP_REPEAT_138(macro) macro(138) PP_REPEAT_137(macro)
#define PP_REPEAT_139(macro) macro(139) PP_REPEAT_138(macro)
#define PP_REPEAT_140(macro) macro(140) PP_REPEAT_139(macro)
#define PP_REPEAT_141(macro) macro(141) PP_REPEAT_140(macro)
#define PP_REPEAT_142(macro) macro(142) PP_REPEAT_141(macro)
#define PP_REPEAT_143(macro) macro(143) PP_REPEAT_142(macro)
#define PP_REPEAT_144(macro) macro(144) PP_REPEAT_143(macro)
#define PP_REPEAT_145(macro) macro(145) PP_REPEAT_144(macro)
#define PP_REPEAT_146(macro) macro(146) PP_REPEAT_145(macro)
#define PP_REPEAT_147(macro) macro(147) PP_REPEAT_146(macro)
#define PP_REPEAT_148(macro) macro(148) PP_REPEAT_147(macro)
#define PP_REPEAT_149(macro) macro(149) PP_REPEAT_148(macro)
#define PP_REPEAT_150(macro) macro(150) PP_REPEAT_149(macro)
#define PP_REPEAT_151(macro) macro(151) PP_REPEAT_150(macro)
#define PP_REPEAT_152(macro) macro(152) PP_REPEAT_151(macro)
#define PP_REPEAT_153(macro) macro(153) PP_REPEAT_152(macro)
#define PP_REPEAT_154(macro) macro(154) PP_REPEAT_153(macro)
#define PP_REPEAT_155(macro) macro(155) PP_REPEAT_154(macro)
#define PP_REPEAT_156(macro) macro(156) PP_REPEAT_155(macro)
#define PP_REPEAT_157(macro) macro(157) PP_REPEAT_156(macro)
#define PP_REPEAT_158(macro) macro(158) PP_REPEAT_157(macro)
#define PP_REPEAT_159(macro) macro(159) PP_REPEAT_158(macro)
#define PP_REPEAT_160(macro) macro(160) PP_REPEAT_159(macro)
#define PP_REPEAT_161(macro) macro(161) PP_REPEAT_160(macro)
#define PP_REPEAT_162(macro) macro(162) PP_REPEAT_161(macro)
#define PP_REPEAT_163(macro) macro(163) PP_REPEAT_162(macro)
#define PP_REPEAT_164(macro) macro(164) PP_REPEAT_163(macro)
#define PP_REPEAT_165(macro) macro(165) PP_REPEAT_164(macro)
#define PP_REPEAT_166(macro) macro(166) PP_REPEAT_165(macro)
#define PP_REPEAT_167(macro) macro(167) PP_REPEAT_166(macro)
#define PP_REPEAT_168(macro) macro(168) PP_REPEAT_167(macro)
#define PP_REPEAT_169(macro) macro(169) PP_REPEAT_168(macro)
#define PP_REPEAT_170(macro) macro(170) PP_REPEAT_169(macro)
#define PP_REPEAT_171(macro) macro(171) PP_REPEAT_170(macro)
#define PP_REPEAT_172(macro) macro(172) PP_REPEAT_171(macro)
#define PP_REPEAT_173(macro) macro(173) PP_REPEAT_172(macro)
#define PP_REPEAT_174(macro) macro(174) PP_REPEAT_173(macro)
#define PP_REPEAT_175(macro) macro(175) PP_REPEAT_174(macro)
#define PP_REPEAT_176(macro) macro(176) PP_REPEAT_175(macro)
#define PP_REPEAT_177(macro) macro(177) PP_REPEAT_176(macro)
#define PP_REPEAT_178(macro) macro(178) PP_REPEAT_177(macro)
#define PP_REPEAT_179(macro) macro(179) PP_REPEAT_178(macro)
#define PP_REPEAT_180(macro) macro(180) PP_REPEAT_179(macro)
#define PP_REPEAT_181(macro) macro(181) PP_REPEAT_180(macro)
#define PP_REPEAT_182(macro) macro(182) PP_REPEAT_181(macro)
#define PP_REPEAT_183(macro) macro(183) PP_REPEAT_182(macro)
#define PP_REPEAT_184(macro) macro(184) PP_REPEAT_183(macro)
#define PP_REPEAT_185(macro) macro(185) PP_REPEAT_184(macro)
#define PP_REPEAT_186(macro) macro(186) PP_REPEAT_185(macro)
#define PP_REPEAT_187(macro) macro(187) PP_REPEAT_186(macro)
#define PP_REPEAT_188(macro) macro(188) PP_REPEAT_187(macro)
#define PP_REPEAT_189(macro) macro(189) PP_REPEAT_188(macro)
#define PP_REPEAT_190(macro) macro(190) PP_REPEAT_189(macro)
#define PP_REPEAT_191(macro) macro(191) PP_REPEAT_190(macro)
#define PP_REPEAT_192(macro) macro(192) PP_REPEAT_191(macro)
#define PP_REPEAT_193(macro) macro(193) PP_REPEAT_192(macro)
#define PP_REPEAT_194(macro) macro(194) PP_REPEAT_193(macro)
#define PP_REPEAT_195(macro) macro(195) PP_REPEAT_194(macro)
#define PP_REPEAT_196(macro) macro(196) PP_REPEAT_195(macro)
#define PP_REPEAT_197(macro) macro(197) PP_REPEAT_196(macro)
#define PP_REPEAT_198(macro) macro(198) PP_REPEAT_197(macro)
#define PP_REPEAT_199(macro) macro(199) PP_REPEAT_198(macro)
#define PP_REPEAT_200(macro) macro(200) PP_REPEAT_199(macro)
#define PP_REPEAT_201(macro) macro(201) PP_REPEAT_200(macro)
#define PP_REPEAT_202(macro) macro(202) PP_REPEAT_201(macro)
#define PP_REPEAT_203(macro) macro(203) PP_REPEAT_202(macro)
#define PP_REPEAT_204(macro) macro(204) PP_REPEAT_203(macro)
#define PP_REPEAT_205(macro) macro(205) PP_REPEAT_204(macro)
#define PP_REPEAT_206(macro) macro(206) PP_REPEAT_205(macro)
#define PP_REPEAT_207(macro) macro(207) PP_REPEAT_206(macro)
#define PP_REPEAT_208(macro) macro(208) PP_REPEAT_207(macro)
#define PP_REPEAT_209(macro) macro(209) PP_REPEAT_208(macro)
#define PP_REPEAT_210(macro) macro(210) PP_REPEAT_209(macro)
#define PP_REPEAT_211(macro) macro(211) PP_REPEAT_210(macro)
#define PP_REPEAT_212(macro) macro(212) PP_REPEAT_211(macro)
#define PP_REPEAT_213(macro) macro(213) PP_REPEAT_212(macro)
#define PP_REPEAT_214(macro) macro(214) PP_REPEAT_213(macro)
#define PP_REPEAT_215(macro) macro(215) PP_REPEAT_214(macro)
#define PP_REPEAT_216(macro) macro(216) PP_REPEAT_215(macro)
#define PP_REPEAT_217(macro) macro(217) PP_REPEAT_216(macro)
#define PP_REPEAT_218(macro) macro(218) PP_REPEAT_217(macro)
#define PP_REPEAT_219(macro) macro(219) PP_REPEAT_218(macro)
#define PP_REPEAT_220(macro) macro(220) PP_REPEAT_219(macro)
#define PP_REPEAT_221(macro) macro(221) PP_REPEAT_220(macro)
#define PP_REPEAT_222(macro) macro(222) PP_REPEAT_221(macro)
#define PP_REPEAT_223(macro) macro(223) PP_REPEAT_222(macro)
#define PP_REPEAT_224(macro) macro(224) PP_REPEAT_223(macro)
#define PP_REPEAT_225(macro) macro(225) PP_REPEAT_224(macro)
#define PP_REPEAT_226(macro) macro(226) PP_REPEAT_225(macro)
#define PP_REPEAT_227(macro) macro(227) PP_REPEAT_226(macro)
#define PP_REPEAT_228(macro) macro(228) PP_REPEAT_227(macro)
#define PP_REPEAT_229(macro) macro(229) PP_REPEAT_228(macro)
#define PP_REPEAT_230(macro) macro(230) PP_REPEAT_229(macro)
#define PP_REPEAT_231(macro) macro(231) PP_REPEAT_230(macro)
#define PP_REPEAT_232(macro) macro(232) PP_REPEAT_231(macro)
#define PP_REPEAT_233(macro) macro(233) PP_REPEAT_232(macro)
#define PP_REPEAT_234(macro) macro(234) PP_REPEAT_233(macro)
#define PP_REPEAT_235(macro) macro(235) PP_REPEAT_234(macro)
#define PP_REPEAT_236(macro) macro(236) PP_REPEAT_235(macro)
#define PP_REPEAT_237(macro) macro(237) PP_REPEAT_236(macro)
#define PP_REPEAT_238(macro) macro(238) PP_REPEAT_237(macro)
#define PP_REPEAT_239(macro) macro(239) PP_REPEAT_238(macro)
#define PP_REPEAT_240(macro) macro(240) PP_REPEAT_239(macro)
#define PP_REPEAT_241(macro) macro(241) PP_REPEAT_240(macro)
#define PP_REPEAT_242(macro) macro(242) PP_REPEAT_241(macro)
#define PP_REPEAT_243(macro) macro(243) PP_REPEAT_242(macro)
#define PP_REPEAT_244(macro) macro(244) PP_REPEAT_243(macro)
#define PP_REPEAT_245(macro) macro(245) PP_REPEAT_244(macro)
#define PP_REPEAT_246(macro) macro(246) PP_REPEAT_245(macro)
#define PP_REPEAT_247(macro) macro(247) PP_REPEAT_246(macro)
#define PP_REPEAT_248(macro) macro(248) PP_REPEAT_247(macro)
#define PP_REPEAT_249(macro) macro(249) PP_REPEAT_248(macro)
#define PP_REPEAT_250(macro) macro(250) PP_REPEAT_249(macro)
#define PP_REPEAT_251(macro) macro(251) PP_REPEAT_250(macro)
#define PP_REPEAT_252(macro) macro(252) PP_REPEAT_251(macro)
#define PP_REPEAT_253(macro) macro(253) PP_REPEAT_252(macro)
#define PP_REPEAT_254(macro) macro(254) PP_REPEAT_253(macro)
#define PP_REPEAT_255(macro) macro(255) PP_REPEAT_254(macro)
#define PP_REPEAT_256(macro) macro(256) PP_REPEAT_255(macro)

#include <stddef.h>

/* ============================================================
 * Code Block Repetition
 * ============================================================ */

#define REPEAT_INDEX(var, count)                                               \
  for (size_t var = 0,                                                         \
              PP_OP_CAT2(_repeat_limit_, __LINE__) = (size_t)(count);          \
       var < PP_OP_CAT2(_repeat_limit_, __LINE__);                             \
       ++var)

#define REPEAT(count)                                                          \
  REPEAT_INDEX(PP_OP_CAT2(_repeat_i_, __LINE__), count)
/* EOF */
