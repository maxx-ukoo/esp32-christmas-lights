//#define NUM_LEDS 150

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_event.h"
#include <stdio.h>
#include "ws2812_control.h"
#include "ledstrip_effect.h"
#include "tcpip_adapter_types.h"

#define ETH_CONFIG_MDC_GPIO     16
#define ETH_CONFIG_MDIO_GPIO    17
#define ETH_CONFIG_PHY_RST_GPIO 0
#define ETH_CONFIG_PHY_ADDR     0

static const char *TAG = "ChLights";

static esp_netif_t *eth_netif = NULL;
static rgbVal *pixels;

static void connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data)
{
    //httpd_handle_t* server = (httpd_handle_t*) arg;
    //if (*server == NULL) {
    //    *server = ui_http_webserver_start(CONFIG_EXAMPLE_WEB_MOUNT_POINT);
    //}
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    //httpd_handle_t* server = (httpd_handle_t*) arg;
    //if (*server) {
    //    httpd_ssl_stop(*server);
    //    *server = NULL;
    //}
}

/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        esp_netif_set_hostname(eth_netif, "ch-lights");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
}

static void eth_start() {
      // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    eth_netif = esp_netif_new(&cfg);
    // Set default handlers to process TCP/IP stuffs
    ESP_ERROR_CHECK(esp_eth_set_default_handlers(eth_netif));
    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    //ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &server));
    //ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &server));

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = ETH_CONFIG_PHY_ADDR;
    // phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO;
    mac_config.smi_mdc_gpio_num = ETH_CONFIG_MDC_GPIO;
    mac_config.smi_mdio_gpio_num = ETH_CONFIG_MDIO_GPIO;
    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_lan8720(&phy_config);
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(esp_eth_driver_install(&config, &eth_handle));
    /* attach Ethernet driver to TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));
    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
}


void random_effects(void *pvParameters){
   int count = 0;
  uint8_t red = 0xff;
  uint8_t green = 0xff;
  uint8_t blue = 0xff;
  while (1) {
    if (count == 0) {
      red = 0xff;
      green = 0x00;
      blue = 0x00;
      count = 1;
    } else if (count == 1)
    {
      red = 0x00;
      green = 0xff;
      blue = 0x00;
      count = 2;
    } else if (count == 2) {
      red = 0x00;
      green = 0x00;
      blue = 0xff;
      count = 3;
    } else {
      red = range_random(0xff);
      blue = range_random(0xff);
      count = range_random(0xff);
    }
    cylonBounce(pixels, NUM_LEDS, red, green, blue, 4, 10, 50);
    colorWipe(pixels, NUM_LEDS, red, green, blue, 50);
    colorWipe(pixels, NUM_LEDS, 0x00, 0x00, 0x00, 50);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 5, 64, true, 30);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 2, 64, true, 10);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 15, 64, true, 10);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 2, 64, true, 20);
    meteorRain(pixels, NUM_LEDS, red, green, blue, 2, 64, true, 5);
    rainbowCycle(pixels, NUM_LEDS, 20);
    RGBLoop(pixels, NUM_LEDS, 5);
  }
}

void app_main()
{
  nvs_flash_init();
  eth_start();
  ws2812_control_init();
  pixels = malloc(sizeof(rgbVal) * NUM_LEDS);
  xTaskCreate(random_effects, "ws2812 random_effects", 4096, NULL, 10, NULL);
  return;
}
