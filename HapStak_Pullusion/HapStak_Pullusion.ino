//  This program was created with reference to the following papers and articles.
//  1)Tweet by @shikky27
//    https://twitter.com/shikky27/status/1463863017259470858
//  2)shikky-lab's blog
//    https://shikky-lab.hatenablog.com/entry/2021/11/26/010513
//  3)TakeshiTanabe et al., "SensoryPropertiesbyUsingaVibrationSpeaker-typeHapticInterface",T.SICE Vol.53 No.1 January 2017
//    https://www.jstage.jst.go.jp/article/sicetr/53/1/53_31/_pdf/-char/ja

#include "M5Atom.h"
#include <driver/i2s.h>

#define CONFIG_I2S_BCK_PIN 19
#define CONFIG_I2S_LRCK_PIN 33
#define CONFIG_I2S_DATA_PIN 22
#define CONFIG_I2S_DATA_IN_PIN 23

#define SPAKER_I2S_NUMBER I2S_NUM_0

extern const unsigned char neg[102400];
extern const unsigned char pos[102400];

int f=0;
bool InitI2SSpaker()
{
    esp_err_t err = ESP_OK;

    i2s_driver_uninstall(SPAKER_I2S_NUMBER);
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
    };
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    i2s_config.use_apll = false;
    i2s_config.tx_desc_auto_clear = true;

    err += i2s_driver_install(SPAKER_I2S_NUMBER, &i2s_config, 0, NULL);
    i2s_pin_config_t tx_pin_config;

#if (ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 3, 0))
    tx_pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
#endif

    tx_pin_config.bck_io_num = CONFIG_I2S_BCK_PIN;
    tx_pin_config.ws_io_num = CONFIG_I2S_LRCK_PIN;
    tx_pin_config.data_out_num = CONFIG_I2S_DATA_PIN;
    tx_pin_config.data_in_num = CONFIG_I2S_DATA_IN_PIN;

    err += i2s_set_pin(SPAKER_I2S_NUMBER, &tx_pin_config);
    err += i2s_set_clk(SPAKER_I2S_NUMBER, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

    return true;
}

size_t bytes_written;

void setup()
{
    M5.begin(true, false, true);
    InitI2SSpaker();
}

void loop()
{  
    M5.update();
    if (M5.Btn.wasPressed())
    {
      if(f==0){
        M5.dis.drawpix(0, 0x00ff00);
        i2s_write(SPAKER_I2S_NUMBER, pos, 102400, &bytes_written, portMAX_DELAY);
        f=1;
        M5.dis.drawpix(0, 0x000000);
      }else{
        M5.dis.drawpix(0, 0x0000ff);
        i2s_write(SPAKER_I2S_NUMBER, neg, 102400, &bytes_written, portMAX_DELAY);
        f=0;
        M5.dis.drawpix(0, 0x000000);
       }
    }
}
