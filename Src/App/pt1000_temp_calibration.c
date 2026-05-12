/**
 * @file pt1000_temp_calibration.c
 * @brief PT1000温度传感器标定实现 - 基于IEC 60751标准
 * 
 * 针对高压项目优化的浮点数版本实现
 * 温度范围：-40°C 到 +85°C
 * 基于bldc+CAN+温度压力标定-电流项目的实现
 */

#include "pt1000.h"

/* ==========================================  Variables  =========================================== */

/**
 * @brief PT1000电阻查找表（浮点版本）从-200°C到+100°C
 * 基于IEC 60751标准，每1°C一个数据点
 */
static const float PT1000_LUT_FLOAT[] = {
    185.2008f, 189.5223f, 193.8402f, 198.1545f, 202.4651f, 206.7722f, 211.0758f, 215.3758f, 
    219.6723f, 223.9653f, 228.2548f, 232.5409f, 236.8236f, 241.1029f, 245.3787f, 249.6513f, 
    253.9205f, 258.1864f, 262.4490f, 266.7083f, 270.9643f, 275.2172f, 279.4668f, 283.7132f, 
    287.9564f, 292.1965f, 296.4335f, 300.6674f, 304.8981f, 309.1258f, 313.3505f, 317.5721f, 
    321.7907f, 326.0063f, 330.2189f, 334.4286f, 338.6353f, 342.8391f, 347.0400f, 351.2381f, 
    355.4333f, 359.6256f, 363.8151f, 368.0018f, 372.1858f, 376.3669f, 380.5453f, 384.7210f, 
    388.8940f, 393.0643f, 397.2318f, 401.3968f, 405.5591f, 409.7187f, 413.8758f, 418.0302f, 
    422.1821f, 426.3314f, 430.4782f, 434.6225f, 438.7642f, 442.9035f, 447.0403f, 451.1746f, 
    455.3065f, 459.4360f, 463.5631f, 467.6877f, 471.8100f, 475.9300f, 480.0475f, 484.1628f, 
    488.2757f, 492.3864f, 496.4947f, 500.6008f, 504.7047f, 508.8063f, 512.9056f, 517.0028f, 
    521.0978f, 525.1906f, 529.2812f, 533.3697f, 537.4560f, 541.5403f, 545.6224f, 549.7024f, 
    553.7804f, 557.8562f, 561.9301f, 566.0018f, 570.0716f, 574.1394f, 578.2051f, 582.2689f, 
    586.3307f, 590.3905f, 594.4484f, 598.5044f, 602.5584f, 606.6105f, 610.6608f, 614.7091f, 
    618.7556f, 622.8002f, 626.8430f, 630.8839f, 634.9230f, 638.9604f, 642.9959f, 647.0296f, 
    651.0615f, 655.0917f, 659.1201f, 663.1468f, 667.1718f, 671.1950f, 675.2165f, 679.2364f, 
    683.2545f, 687.2710f, 691.2858f, 695.2989f, 699.3104f, 703.3202f, 707.3285f, 711.3351f, 
    715.3401f, 719.3435f, 723.3453f, 727.3456f, 731.3443f, 735.3414f, 739.3370f, 743.3310f, 
    747.3235f, 751.3145f, 755.3040f, 759.2920f, 763.2784f, 767.2634f, 771.2469f, 775.2290f, 
    779.2096f, 783.1887f, 787.1664f, 791.1426f, 795.1174f, 799.0908f, 803.0628f, 807.0334f, 
    811.0026f, 814.9704f, 818.9368f, 822.9018f, 826.8654f, 830.8277f, 834.7887f, 838.7483f, 
    842.7065f, 846.6634f, 850.6190f, 854.5733f, 858.5262f, 862.4779f, 866.4282f, 870.3772f, 
    874.3249f, 878.2714f, 882.2166f, 886.1605f, 890.1031f, 894.0444f, 897.9845f, 901.9234f, 
    905.8610f, 909.7973f, 913.7325f, 917.6663f, 921.5990f, 925.5304f, 929.4606f, 933.3896f, 
    937.3174f, 941.2439f, 945.1693f, 949.0935f, 953.0164f, 956.9382f, 960.8588f, 964.7782f, 
    968.6964f, 972.6134f, 976.5293f, 980.4440f, 984.3575f, 988.2699f, 992.1811f, 996.0911f, 
    1000.0000f, 1003.9077f, 1007.8143f, 1011.7197f, 1015.6240f, 1019.5271f, 1023.4290f, 1027.3298f, 
    1031.2294f, 1035.1279f, 1039.0252f, 1042.9214f, 1046.8164f, 1050.7103f, 1054.6030f, 1058.4946f, 
    1062.3850f, 1066.2742f, 1070.1623f, 1074.0492f, 1077.9350f, 1081.8196f, 1085.7031f, 1089.5854f, 
    1093.4666f, 1097.3466f, 1101.2254f, 1105.1031f, 1108.9796f, 1112.8550f, 1116.7292f, 1120.6023f, 
    1124.4742f, 1128.3450f, 1132.2146f, 1136.0831f, 1139.9504f, 1143.8165f, 1147.6815f, 1151.5453f, 
    1155.4080f, 1159.2695f, 1163.1299f, 1166.9891f, 1170.8472f, 1174.7041f, 1178.5598f, 1182.4144f, 
    1186.2678f, 1190.1201f, 1193.9713f, 1197.8212f, 1201.6700f, 1205.5177f, 1209.3642f, 1213.2096f, 
    1217.0538f, 1220.8968f, 1224.7387f, 1228.5794f, 1232.4190f, 1236.2574f, 1240.0947f, 1243.9308f, 
    1247.7658f, 1251.5996f, 1255.4322f, 1259.2637f, 1263.0940f, 1266.9232f, 1270.7513f, 1274.5781f, 
    1278.4038f, 1282.2284f, 1286.0518f, 1289.8741f, 1293.6952f, 1297.5151f, 1301.3339f, 1305.1515f, 
    1308.9680f, 1312.7833f, 1316.5975f, 1320.4105f, 1324.2224f, 1328.0331f, 1331.8426f, 1335.6510f, 
    1339.4582f, 1343.2643f, 1347.0693f, 1350.8730f, 1354.6756f, 1358.4771f, 1362.2774f, 1366.0766f, 
    1369.8746f, 1373.6714f, 1377.4671f, 1381.2616f, 1385.0550f, 
};

/**
 * @brief PT1000电阻查找表（定点整数版本）从-200°C到+100°C
 * 注意：电阻值已放大100倍
 */
static const uint32_t PT1000_LUT_INT[] = {
    18520U, 18952U, 19384U, 19815U, 20246U, 20677U, 21107U, 21537U, 
    21967U, 22396U, 22825U, 23254U, 23682U, 24110U, 24537U, 24965U, 
    25392U, 25818U, 26244U, 26670U, 27096U, 27521U, 27946U, 28371U, 
    28795U, 29219U, 29643U, 30066U, 30489U, 30912U, 31335U, 31757U, 
    32179U, 32600U, 33021U, 33442U, 33863U, 34283U, 34704U, 35123U, 
    35543U, 35962U, 36381U, 36800U, 37218U, 37636U, 38054U, 38472U, 
    38889U, 39306U, 39723U, 40139U, 40555U, 40971U, 41387U, 41803U, 
    42218U, 42633U, 43047U, 43462U, 43876U, 44290U, 44704U, 45117U, 
    45530U, 45943U, 46356U, 46768U, 47181U, 47592U, 48004U, 48416U, 
    48827U, 49238U, 49649U, 50060U, 50470U, 50880U, 51290U, 51700U, 
    52109U, 52519U, 52928U, 53336U, 53745U, 54154U, 54562U, 54970U, 
    55378U, 55785U, 56193U, 56600U, 57007U, 57413U, 57820U, 58226U, 
    58633U, 59039U, 59444U, 59850U, 60255U, 60661U, 61066U, 61470U, 
    61875U, 62280U, 62684U, 63088U, 63492U, 63896U, 64299U, 64702U, 
    65106U, 65509U, 65912U, 66314U, 66717U, 67119U, 67521U, 67923U, 
    68325U, 68727U, 69128U, 69529U, 69931U, 70332U, 70732U, 71133U, 
    71534U, 71934U, 72334U, 72734U, 73134U, 73534U, 73933U, 74333U, 
    74732U, 75131U, 75530U, 75929U, 76327U, 76726U, 77124U, 77522U, 
    77920U, 78318U, 78716U, 79114U, 79511U, 79909U, 80306U, 80703U, 
    81100U, 81497U, 81893U, 82290U, 82686U, 83082U, 83478U, 83874U, 
    84270U, 84666U, 85061U, 85457U, 85852U, 86247U, 86642U, 87037U, 
    87432U, 87827U, 88221U, 88616U, 89010U, 89404U, 89798U, 90192U, 
    90586U, 90979U, 91373U, 91766U, 92159U, 92553U, 92946U, 93338U, 
    93731U, 94124U, 94516U, 94909U, 95301U, 95693U, 96085U, 96477U, 
    96869U, 97261U, 97652U, 98044U, 98435U, 98826U, 99218U, 99609U, 
    100000U, 100390U, 100781U, 101171U, 101562U, 101952U, 102342U, 102732U, 
    103122U, 103512U, 103902U, 104292U, 104681U, 105071U, 105460U, 105849U, 
    106238U, 106627U, 107016U, 107404U, 107793U, 108181U, 108570U, 108958U, 
    109346U, 109734U, 110122U, 110510U, 110897U, 111285U, 111672U, 112060U, 
    112447U, 112834U, 113221U, 113608U, 113995U, 114381U, 114768U, 115154U, 
};

/**
 * @brief 根据测量的电阻值计算温度，使用二分查找+线性插值
 * @param resistance 测得的PT1000电阻值 (单位: Ohm)
 * @return float     计算出的温度值 (单位: °C)
 */
float pt1000_get_temp_float(float resistance) {
    // 1. 边界检查
    if (resistance < PT1000_LUT_FLOAT[0] || 
        resistance > PT1000_LUT_FLOAT[LUT_TABLE_SIZE - 1]) {
        return PT1000_RESISTANCE_OUT_OF_RANGE_F;
    }

    // 2. 二分法查找电阻所在的区间索引
    uint16_t low = 0;
    uint16_t high = LUT_TABLE_SIZE - 1;
    uint16_t mid;

    while (low <= high) {
        mid = low + (high - low) / 2;
        if (resistance < PT1000_LUT_FLOAT[mid]) {
            high = mid - 1;
        } else if (resistance > PT1000_LUT_FLOAT[mid]) {
            low = mid + 1;
        } else {
            // 精确匹配
            return (float)(LUT_TEMP_START_C + mid * LUT_TEMP_STEP_C);
        }
    }
    
    uint16_t index_low = high;
    
    // 防止数组越界
    if (index_low >= LUT_TABLE_SIZE - 1) {
        index_low = LUT_TABLE_SIZE - 2;
    }

    // 3. 分段线性插值
    float r1 = PT1000_LUT_FLOAT[index_low];
    float r2 = PT1000_LUT_FLOAT[index_low + 1];
    
    float t1 = (float)(LUT_TEMP_START_C + index_low * LUT_TEMP_STEP_C);
    float t2 = (float)(LUT_TEMP_START_C + (index_low + 1) * LUT_TEMP_STEP_C);

    // 线性插值公式: T = T1 + (R-R1)/(R2-R1) * (T2-T1)
    float temperature = t1 + (t2 - t1) * (resistance - r1) / (r2 - r1);

    return temperature;
}

/**
 * @brief 电压转PT1000电阻值（基于实测标定数据）
 * 使用14个实测标定点进行分段线性插值
 * 与bldc项目保持一致的实现
 */
float pt1000_voltage_to_resistance(float voltage)
{
    // 实测电压-电阻标定数据表（基于电阻箱实测，14个标定点）
    typedef struct {
        float voltage;    // 测得的电压值 (V)
        float resistance; // 对应的电阻值 (Ω)
        float temperature; // 对应的温度值 (°C) - 仅用于注释
    } voltage_resistance_point_t;

    // 基于电阻箱实测的标定点（电压从小到大排序）- -40°C到90°C量程
    const voltage_resistance_point_t calib_points[] = {
        {0.224f,  845.0f,  -40.0f}, // 基于实测平均值
        {0.552f,  884.5f,  -30.0f}, // 基于实测平均值
        {0.887f,  923.5f,  -20.0f}, // 基于实测平均值
        {1.210f,  963.0f,  -10.0f}, // 基于实测平均值
        {1.523f,  1002.0f,   0.0f}, // 基于实测平均值
        {1.838f,  1040.5f,  10.0f}, // 基于实测平均值
        {2.159f,  1080.0f,  20.0f}, // 基于实测平均值
        {2.372f,  1113.0f,  30.0f}, // 基于实测平均值
        {2.455f,  1145.0f,  40.0f}, // 基于实测平均值
        {2.547f,  1186.0f,  50.0f}, // 基于实测平均值
        {2.657f,  1229.0f,  60.0f}, // 基于实测平均值
        {2.752f,  1263.0f,  70.0f}, // 基于实测平均值
        {2.867f,  1303.0f,  80.0f}, // 基于实测平均值
        {2.977f,  1341.0f,  90.0f}, // 基于实测平均值
    };

    const int num_points = sizeof(calib_points) / sizeof(calib_points[0]);

    // 边界处理 - 低于最低标定点
    if (voltage <= calib_points[0].voltage) {
        // 用第一段斜率外推
        float slope = (calib_points[1].resistance - calib_points[0].resistance) /
                     (calib_points[1].voltage - calib_points[0].voltage);
        float resistance = calib_points[0].resistance + slope * (voltage - calib_points[0].voltage);
        // 确保电阻值在合理范围内，对应温度约为-60°C
        return (resistance < 700.0f) ? 700.0f : resistance;
    }
    
    // 边界处理 - 高于最高标定点  
    if (voltage >= calib_points[num_points-1].voltage) {
        // 用最后一段斜率外推
        float slope = (calib_points[num_points-1].resistance - calib_points[num_points-2].resistance) /
                     (calib_points[num_points-1].voltage - calib_points[num_points-2].voltage);
        float resistance = calib_points[num_points-1].resistance + slope * (voltage - calib_points[num_points-1].voltage);
        return (resistance > 2000.0f) ? 2000.0f : resistance; // 最大电阻限制
    }
    
    // 在标定范围内，找到对应的区间进行线性插值
    for (int i = 0; i < num_points - 1; i++) {
        if (voltage >= calib_points[i].voltage && voltage <= calib_points[i+1].voltage) {
            // 线性插值计算电阻值
            float slope = (calib_points[i+1].resistance - calib_points[i].resistance) /
                         (calib_points[i+1].voltage - calib_points[i].voltage);
            return calib_points[i].resistance + slope * (voltage - calib_points[i].voltage);
        }
    }
    
    // 理论上不应该到达这里，返回默认值
    return 1000.0f; // 0°C对应的标准电阻值
}

/**
 * @brief 根据电压直接计算温度
 * 使用电压→电阻→温度两步转换法
 * 与bldc项目保持一致的实现
 */
float pt1000_voltage_to_temp_float(float voltage_v) {
    // 第一步：电压转电阻（基于实测标定数据）
    float resistance = pt1000_voltage_to_resistance(voltage_v);
    
    // 第二步：电阻转温度（基于PT1000标准分度表）
    float temperature = pt1000_get_temp_float(resistance);
    
    // 检查是否超出有效范围
    if (temperature == PT1000_RESISTANCE_OUT_OF_RANGE_F) {
        return -999.0f; // 返回错误值
    }
    
    return temperature;
}

/**
 * @brief 根据ADC原始值计算温度
 * 使用电压→电阻→温度两步转换法
 * 与bldc项目保持一致的实现
 * 
 * @param adc_raw    ADC原始采样值 (0 ~ 2^adc_bits - 1)
 * @param adc_ref_mv ADC参考电压 (单位: mV)，通常为5000mV
 * @param adc_bits   ADC分辨率位数 (如12位ADC则为12)
 * @return float     计算出的温度值 (单位: °C)
 */
float pt1000_adc_to_temp_float(uint32_t adc_raw, float adc_ref_mv, uint8_t adc_bits) {
    // 1. 将ADC原始值转换为电压 (V)
    float adc_voltage_v = ((float)adc_raw * adc_ref_mv / 1000.0f) / (float)((1UL << adc_bits) - 1);
    
    // 2. 使用电压→电阻→温度两步转换法
    return pt1000_voltage_to_temp_float(adc_voltage_v);
}

/**
 * @brief 获取PT1000电阻查找表（浮点版本）
 * @return const float* 电阻查找表指针
 */
const float* pt1000_get_resistance_lut(void) {
    return PT1000_LUT_FLOAT;
}

/**
 * @brief 获取PT1000电阻查找表（浮点版本）
 * @return const float* 电阻查找表指针
 */
const float* pt1000_get_lut_float(void) {
    return PT1000_LUT_FLOAT;
}

/**
 * @brief 获取PT1000电阻查找表（定点整数版本）
 * @return const uint32_t* 电阻查找表指针
 */
const uint32_t* pt1000_get_lut_int(void) {
    return PT1000_LUT_INT;
} 
