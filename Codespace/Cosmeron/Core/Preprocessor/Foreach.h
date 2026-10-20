#pragma once

#include "Compiling.inc"
#include "Arguments.h"
#include "Operations.h"

#define PP_FOREACH(macro, ...)                                                \
  PP_OP_CAT2(PP_FOREACH_, PP_ARG_COUNT(__VA_ARGS__))(macro, __VA_ARGS__)

#define PP_FOREACH_1(macro, a1) macro(a1)
#define PP_FOREACH_2(macro, a1, ...) macro(a1) PP_FOREACH_1(macro, __VA_ARGS__)
#define PP_FOREACH_3(macro, a1, ...) macro(a1) PP_FOREACH_2(macro, __VA_ARGS__)
#define PP_FOREACH_4(macro, a1, ...) macro(a1) PP_FOREACH_3(macro, __VA_ARGS__)
#define PP_FOREACH_5(macro, a1, ...) macro(a1) PP_FOREACH_4(macro, __VA_ARGS__)
#define PP_FOREACH_6(macro, a1, ...) macro(a1) PP_FOREACH_5(macro, __VA_ARGS__)
#define PP_FOREACH_7(macro, a1, ...) macro(a1) PP_FOREACH_6(macro, __VA_ARGS__)
#define PP_FOREACH_8(macro, a1, ...) macro(a1) PP_FOREACH_7(macro, __VA_ARGS__)
#define PP_FOREACH_9(macro, a1, ...) macro(a1) PP_FOREACH_8(macro, __VA_ARGS__)
#define PP_FOREACH_10(macro, a1, ...) macro(a1) PP_FOREACH_9(macro, __VA_ARGS__)
#define PP_FOREACH_11(macro, a1, ...) macro(a1) PP_FOREACH_10(macro, __VA_ARGS__)
#define PP_FOREACH_12(macro, a1, ...) macro(a1) PP_FOREACH_11(macro, __VA_ARGS__)
#define PP_FOREACH_13(macro, a1, ...) macro(a1) PP_FOREACH_12(macro, __VA_ARGS__)
#define PP_FOREACH_14(macro, a1, ...) macro(a1) PP_FOREACH_13(macro, __VA_ARGS__)
#define PP_FOREACH_15(macro, a1, ...) macro(a1) PP_FOREACH_14(macro, __VA_ARGS__)
#define PP_FOREACH_16(macro, a1, ...) macro(a1) PP_FOREACH_15(macro, __VA_ARGS__)
#define PP_FOREACH_17(macro, a1, ...) macro(a1) PP_FOREACH_16(macro, __VA_ARGS__)
#define PP_FOREACH_18(macro, a1, ...) macro(a1) PP_FOREACH_17(macro, __VA_ARGS__)
#define PP_FOREACH_19(macro, a1, ...) macro(a1) PP_FOREACH_18(macro, __VA_ARGS__)
#define PP_FOREACH_20(macro, a1, ...) macro(a1) PP_FOREACH_19(macro, __VA_ARGS__)
#define PP_FOREACH_21(macro, a1, ...) macro(a1) PP_FOREACH_20(macro, __VA_ARGS__)
#define PP_FOREACH_22(macro, a1, ...) macro(a1) PP_FOREACH_21(macro, __VA_ARGS__)
#define PP_FOREACH_23(macro, a1, ...) macro(a1) PP_FOREACH_22(macro, __VA_ARGS__)
#define PP_FOREACH_24(macro, a1, ...) macro(a1) PP_FOREACH_23(macro, __VA_ARGS__)
#define PP_FOREACH_25(macro, a1, ...) macro(a1) PP_FOREACH_24(macro, __VA_ARGS__)
#define PP_FOREACH_26(macro, a1, ...) macro(a1) PP_FOREACH_25(macro, __VA_ARGS__)
#define PP_FOREACH_27(macro, a1, ...) macro(a1) PP_FOREACH_26(macro, __VA_ARGS__)
#define PP_FOREACH_28(macro, a1, ...) macro(a1) PP_FOREACH_27(macro, __VA_ARGS__)
#define PP_FOREACH_29(macro, a1, ...) macro(a1) PP_FOREACH_28(macro, __VA_ARGS__)
#define PP_FOREACH_30(macro, a1, ...) macro(a1) PP_FOREACH_29(macro, __VA_ARGS__)
#define PP_FOREACH_31(macro, a1, ...) macro(a1) PP_FOREACH_30(macro, __VA_ARGS__)
#define PP_FOREACH_32(macro, a1, ...) macro(a1) PP_FOREACH_31(macro, __VA_ARGS__)
#define PP_FOREACH_33(macro, a1, ...) macro(a1) PP_FOREACH_32(macro, __VA_ARGS__)
#define PP_FOREACH_34(macro, a1, ...) macro(a1) PP_FOREACH_33(macro, __VA_ARGS__)
#define PP_FOREACH_35(macro, a1, ...) macro(a1) PP_FOREACH_34(macro, __VA_ARGS__)
#define PP_FOREACH_36(macro, a1, ...) macro(a1) PP_FOREACH_35(macro, __VA_ARGS__)
#define PP_FOREACH_37(macro, a1, ...) macro(a1) PP_FOREACH_36(macro, __VA_ARGS__)
#define PP_FOREACH_38(macro, a1, ...) macro(a1) PP_FOREACH_37(macro, __VA_ARGS__)
#define PP_FOREACH_39(macro, a1, ...) macro(a1) PP_FOREACH_38(macro, __VA_ARGS__)
#define PP_FOREACH_40(macro, a1, ...) macro(a1) PP_FOREACH_39(macro, __VA_ARGS__)
#define PP_FOREACH_41(macro, a1, ...) macro(a1) PP_FOREACH_40(macro, __VA_ARGS__)
#define PP_FOREACH_42(macro, a1, ...) macro(a1) PP_FOREACH_41(macro, __VA_ARGS__)
#define PP_FOREACH_43(macro, a1, ...) macro(a1) PP_FOREACH_42(macro, __VA_ARGS__)
#define PP_FOREACH_44(macro, a1, ...) macro(a1) PP_FOREACH_43(macro, __VA_ARGS__)
#define PP_FOREACH_45(macro, a1, ...) macro(a1) PP_FOREACH_44(macro, __VA_ARGS__)
#define PP_FOREACH_46(macro, a1, ...) macro(a1) PP_FOREACH_45(macro, __VA_ARGS__)
#define PP_FOREACH_47(macro, a1, ...) macro(a1) PP_FOREACH_46(macro, __VA_ARGS__)
#define PP_FOREACH_48(macro, a1, ...) macro(a1) PP_FOREACH_47(macro, __VA_ARGS__)
#define PP_FOREACH_49(macro, a1, ...) macro(a1) PP_FOREACH_48(macro, __VA_ARGS__)
#define PP_FOREACH_50(macro, a1, ...) macro(a1) PP_FOREACH_49(macro, __VA_ARGS__)
#define PP_FOREACH_51(macro, a1, ...) macro(a1) PP_FOREACH_50(macro, __VA_ARGS__)
#define PP_FOREACH_52(macro, a1, ...) macro(a1) PP_FOREACH_51(macro, __VA_ARGS__)
#define PP_FOREACH_53(macro, a1, ...) macro(a1) PP_FOREACH_52(macro, __VA_ARGS__)
#define PP_FOREACH_54(macro, a1, ...) macro(a1) PP_FOREACH_53(macro, __VA_ARGS__)
#define PP_FOREACH_55(macro, a1, ...) macro(a1) PP_FOREACH_54(macro, __VA_ARGS__)
#define PP_FOREACH_56(macro, a1, ...) macro(a1) PP_FOREACH_55(macro, __VA_ARGS__)
#define PP_FOREACH_57(macro, a1, ...) macro(a1) PP_FOREACH_56(macro, __VA_ARGS__)
#define PP_FOREACH_58(macro, a1, ...) macro(a1) PP_FOREACH_57(macro, __VA_ARGS__)
#define PP_FOREACH_59(macro, a1, ...) macro(a1) PP_FOREACH_58(macro, __VA_ARGS__)
#define PP_FOREACH_60(macro, a1, ...) macro(a1) PP_FOREACH_59(macro, __VA_ARGS__)
#define PP_FOREACH_61(macro, a1, ...) macro(a1) PP_FOREACH_60(macro, __VA_ARGS__)
#define PP_FOREACH_62(macro, a1, ...) macro(a1) PP_FOREACH_61(macro, __VA_ARGS__)
#define PP_FOREACH_63(macro, a1, ...) macro(a1) PP_FOREACH_62(macro, __VA_ARGS__)
#define PP_FOREACH_64(macro, a1, ...) macro(a1) PP_FOREACH_63(macro, __VA_ARGS__)
#define PP_FOREACH_65(macro, a1, ...) macro(a1) PP_FOREACH_64(macro, __VA_ARGS__)
#define PP_FOREACH_66(macro, a1, ...) macro(a1) PP_FOREACH_65(macro, __VA_ARGS__)
#define PP_FOREACH_67(macro, a1, ...) macro(a1) PP_FOREACH_66(macro, __VA_ARGS__)
#define PP_FOREACH_68(macro, a1, ...) macro(a1) PP_FOREACH_67(macro, __VA_ARGS__)
#define PP_FOREACH_69(macro, a1, ...) macro(a1) PP_FOREACH_68(macro, __VA_ARGS__)
#define PP_FOREACH_70(macro, a1, ...) macro(a1) PP_FOREACH_69(macro, __VA_ARGS__)
#define PP_FOREACH_71(macro, a1, ...) macro(a1) PP_FOREACH_70(macro, __VA_ARGS__)
#define PP_FOREACH_72(macro, a1, ...) macro(a1) PP_FOREACH_71(macro, __VA_ARGS__)
#define PP_FOREACH_73(macro, a1, ...) macro(a1) PP_FOREACH_72(macro, __VA_ARGS__)
#define PP_FOREACH_74(macro, a1, ...) macro(a1) PP_FOREACH_73(macro, __VA_ARGS__)
#define PP_FOREACH_75(macro, a1, ...) macro(a1) PP_FOREACH_74(macro, __VA_ARGS__)
#define PP_FOREACH_76(macro, a1, ...) macro(a1) PP_FOREACH_75(macro, __VA_ARGS__)
#define PP_FOREACH_77(macro, a1, ...) macro(a1) PP_FOREACH_76(macro, __VA_ARGS__)
#define PP_FOREACH_78(macro, a1, ...) macro(a1) PP_FOREACH_77(macro, __VA_ARGS__)
#define PP_FOREACH_79(macro, a1, ...) macro(a1) PP_FOREACH_78(macro, __VA_ARGS__)
#define PP_FOREACH_80(macro, a1, ...) macro(a1) PP_FOREACH_79(macro, __VA_ARGS__)
#define PP_FOREACH_81(macro, a1, ...) macro(a1) PP_FOREACH_80(macro, __VA_ARGS__)
#define PP_FOREACH_82(macro, a1, ...) macro(a1) PP_FOREACH_81(macro, __VA_ARGS__)
#define PP_FOREACH_83(macro, a1, ...) macro(a1) PP_FOREACH_82(macro, __VA_ARGS__)
#define PP_FOREACH_84(macro, a1, ...) macro(a1) PP_FOREACH_83(macro, __VA_ARGS__)
#define PP_FOREACH_85(macro, a1, ...) macro(a1) PP_FOREACH_84(macro, __VA_ARGS__)
#define PP_FOREACH_86(macro, a1, ...) macro(a1) PP_FOREACH_85(macro, __VA_ARGS__)
#define PP_FOREACH_87(macro, a1, ...) macro(a1) PP_FOREACH_86(macro, __VA_ARGS__)
#define PP_FOREACH_88(macro, a1, ...) macro(a1) PP_FOREACH_87(macro, __VA_ARGS__)
#define PP_FOREACH_89(macro, a1, ...) macro(a1) PP_FOREACH_88(macro, __VA_ARGS__)
#define PP_FOREACH_90(macro, a1, ...) macro(a1) PP_FOREACH_89(macro, __VA_ARGS__)
#define PP_FOREACH_91(macro, a1, ...) macro(a1) PP_FOREACH_90(macro, __VA_ARGS__)
#define PP_FOREACH_92(macro, a1, ...) macro(a1) PP_FOREACH_91(macro, __VA_ARGS__)
#define PP_FOREACH_93(macro, a1, ...) macro(a1) PP_FOREACH_92(macro, __VA_ARGS__)
#define PP_FOREACH_94(macro, a1, ...) macro(a1) PP_FOREACH_93(macro, __VA_ARGS__)
#define PP_FOREACH_95(macro, a1, ...) macro(a1) PP_FOREACH_94(macro, __VA_ARGS__)
#define PP_FOREACH_96(macro, a1, ...) macro(a1) PP_FOREACH_95(macro, __VA_ARGS__)
#define PP_FOREACH_97(macro, a1, ...) macro(a1) PP_FOREACH_96(macro, __VA_ARGS__)
#define PP_FOREACH_98(macro, a1, ...) macro(a1) PP_FOREACH_97(macro, __VA_ARGS__)
#define PP_FOREACH_99(macro, a1, ...) macro(a1) PP_FOREACH_98(macro, __VA_ARGS__)
#define PP_FOREACH_100(macro, a1, ...) macro(a1) PP_FOREACH_99(macro, __VA_ARGS__)

/* Foreach with context parameter passed to macro(ctx, item) */
#define PP_FOREACH_CTX(macro, ctx, ...) \
  PP_OP_CAT2(PP_FOREACH_CTX_, PP_ARG_COUNT(__VA_ARGS__))(macro, ctx, __VA_ARGS__)

#define PP_FOREACH_CTX_1(macro, ctx, a1) macro(ctx, a1)
#define PP_FOREACH_CTX_2(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_1(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_3(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_2(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_4(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_3(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_5(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_4(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_6(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_5(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_7(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_6(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_8(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_7(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_9(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_8(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_10(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_9(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_11(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_10(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_12(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_11(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_13(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_12(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_14(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_13(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_15(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_14(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_16(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_15(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_17(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_16(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_18(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_17(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_19(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_18(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_20(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_19(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_21(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_20(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_22(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_21(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_23(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_22(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_24(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_23(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_25(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_24(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_26(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_25(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_27(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_26(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_28(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_27(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_29(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_28(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_30(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_29(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_31(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_30(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_32(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_31(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_33(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_32(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_34(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_33(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_35(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_34(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_36(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_35(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_37(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_36(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_38(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_37(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_39(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_38(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_40(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_39(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_41(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_40(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_42(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_41(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_43(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_42(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_44(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_43(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_45(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_44(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_46(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_45(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_47(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_46(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_48(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_47(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_49(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_48(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_50(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_49(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_51(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_50(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_52(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_51(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_53(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_52(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_54(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_53(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_55(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_54(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_56(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_55(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_57(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_56(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_58(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_57(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_59(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_58(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_60(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_59(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_61(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_60(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_62(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_61(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_63(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_62(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_64(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_63(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_65(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_64(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_66(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_65(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_67(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_66(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_68(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_67(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_69(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_68(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_70(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_69(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_71(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_70(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_72(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_71(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_73(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_72(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_74(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_73(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_75(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_74(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_76(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_75(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_77(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_76(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_78(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_77(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_79(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_78(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_80(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_79(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_81(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_80(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_82(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_81(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_83(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_82(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_84(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_83(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_85(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_84(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_86(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_85(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_87(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_86(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_88(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_87(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_89(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_88(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_90(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_89(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_91(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_90(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_92(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_91(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_93(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_92(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_94(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_93(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_95(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_94(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_96(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_95(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_97(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_96(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_98(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_97(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_99(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_98(macro, ctx, __VA_ARGS__)
#define PP_FOREACH_CTX_100(macro, ctx, a1, ...) \
  macro(ctx, a1) PP_FOREACH_CTX_99(macro, ctx, __VA_ARGS__)

/* Convert parameter declaration lists (e.g. (int a, void *b)) to struct fields using Foreach */
#define COSMERON_MACRO_INTERNAL_PARAM_TO_FIELD(item) item;
#define COSMERON_MACRO_INTERNAL_PTF_EMPTY_1(...) int _unused;
#define COSMERON_MACRO_INTERNAL_PTF_EMPTY_0(...)                               \
  PP_FOREACH(COSMERON_MACRO_INTERNAL_PARAM_TO_FIELD, __VA_ARGS__)

#undef PP_PARAMS_TO_FIELDS
#define PP_PARAMS_TO_FIELDS(PARAMS)                                            \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_PTF_EMPTY_,                               \
             PP_IS_EMPTY_PARAMS(PARAMS))(PP_UNWRAP PARAMS)

/* EOF */
