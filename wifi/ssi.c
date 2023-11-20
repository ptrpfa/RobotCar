#include "ssi.h"

const char *ssi_tags[] = {"volt", "temp", "led", "width", "height", "gridmap"};

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen)
{
  size_t printed;
  printf("iInsertLen\n");
  printf("%d\n", iInsertLen);
  switch (iIndex)
  {
  case 0: // volt
  {
    const float voltage = adc_read() * 3.3f / (1 << 12);
    printed = snprintf(pcInsert, iInsertLen, "%f", voltage);
  }
  break;
  case 1: // temp
  {
    const float voltage = adc_read() * 3.3f / (1 << 12);
    const float tempC = 27.0f - (voltage - 0.706f) / 0.001721f;
    printed = snprintf(pcInsert, iInsertLen, "%f", tempC);
  }
  break;
  case 2: // led
  {
    bool led_status = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
    if (led_status == true)
    {
      printed = snprintf(pcInsert, iInsertLen, "ON");
    }
    else
    {
      printed = snprintf(pcInsert, iInsertLen, "OFF");
    }
  }
  break;
  case 3: // maxwidth
  {
    printed = snprintf(pcInsert, iInsertLen, "%d", 8); // Import max width details
  }
  break;
  case 4: // maxheight
  {
    printed = snprintf(pcInsert, iInsertLen, "%d", 8); // Import max height details
  }
  break;
  case 5: // gridmap
  {
    const char* test_data = "[{'x':0,'y':0,'n':1,'s':0,'e':1,'w':1},{'x':0,'y':1,'n':1,'s':0,'e':1,'w':1},{'x':0,'y':2,'n':0,'s':1,'e':1,'w':1},{'x':0,'y':3,'n':1,'s':1,'e':1,'w':0},{'x':0,'y':4,'n':0,'s':1,'e':1,'w':0},{'x':0,'y':5,'n':1,'s':0,'e':1,'w':0},{'x':0,'y':6,'n':1,'s':0,'e':0,'w':1},{'x':0,'y':7,'n':1,'s':0,'e':0,'w':0},{'x':1,'y':0,'n':1,'s':0,'e':1,'w':1},{'x':1,'y':1,'n':1,'s':0,'e':0,'w':1},{'x':1,'y':2,'n':0,'s':1,'e':0,'w':0},{'x':1,'y':3,'n':1,'s':0,'e':1,'w':0},{'x':1,'y':4,'n':1,'s':0,'e':0,'w':1},{'x':1,'y':5,'n':1,'s':1,'e':0,'w':0},{'x':1,'y':6,'n':1,'s':1,'e':1,'w':0},{'x':1,'y':7,'n':0,'s':0,'e':0,'w':1},{'x':2,'y':0,'n':0,'s':0,'e':0,'w':1},{'x':2,'y':1,'n':1,'s':0,'e':0,'w':1},{'x':2,'y':2,'n':1,'s':0,'e':1,'w':0},{'x':2,'y':3,'n':1,'s':1,'e':0,'w':1},{'x':2,'y':4,'n':1,'s':0,'e':1,'w':1},{'x':2,'y':5,'n':1,'s':1,'e':0,'w':0},{'x':2,'y':6,'n':1,'s':0,'e':1,'w':0},{'x':2,'y':7,'n':0,'s':0,'e':1,'w':1},{'x':3,'y':0,'n':1,'s':0,'e':0,'w':0},{'x':3,'y':1,'n':1,'s':1,'e':1,'w':0},{'x':3,'y':2,'n':1,'s':1,'e':0,'w':1},{'x':3,'y':3,'n':0,'s':1,'e':0,'w':0},{'x':3,'y':4,'n':0,'s':1,'e':1,'w':0},{'x':3,'y':5,'n':1,'s':1,'e':0,'w':1},{'x':3,'y':6,'n':0,'s':0,'e':0,'w':0},{'x':3,'y':7,'n':0,'s':0,'e':0,'w':0},{'x':4,'y':0,'n':0,'s':0,'e':0,'w':1},{'x':4,'y':1,'n':1,'s':0,'e':0,'w':1},{'x':4,'y':2,'n':0,'s':1,'e':0,'w':1},{'x':4,'y':3,'n':0,'s':1,'e':1,'w':0},{'x':4,'y':4,'n':0,'s':1,'e':1,'w':0},{'x':4,'y':5,'n':0,'s':0,'e':1,'w':0},{'x':4,'y':6,'n':0,'s':0,'e':1,'w':0},{'x':4,'y':7,'n':1,'s':0,'e':1,'w':1},{'x':5,'y':0,'n':1,'s':0,'e':0,'w':0},{'x':5,'y':1,'n':0,'s':1,'e':1,'w':0},{'x':5,'y':2,'n':1,'s':1,'e':1,'w':1},{'x':5,'y':3,'n':0,'s':1,'e':0,'w':0},{'x':5,'y':4,'n':1,'s':1,'e':1,'w':1},{'x':5,'y':5,'n':0,'s':1,'e':0,'w':1},{'x':5,'y':6,'n':0,'s':0,'e':0,'w':0},{'x':5,'y':7,'n':0,'s':1,'e':0,'w':1},{'x':6,'y':0,'n':1,'s':0,'e':0,'w':0},{'x':6,'y':1,'n':0,'s':1,'e':1,'w':1},{'x':6,'y':2,'n':1,'s':0,'e':0,'w':1},{'x':6,'y':3,'n':0,'s':1,'e':1,'w':1},{'x':6,'y':4,'n':1,'s':0,'e':1,'w':1},{'x':6,'y':5,'n':0,'s':1,'e':0,'w':0},{'x':6,'y':6,'n':0,'s':1,'e':1,'w':1},{'x':6,'y':7,'n':0,'s':1,'e':0,'w':1},{'x':7,'y':0,'n':0,'s':1,'e':1,'w':1},{'x':7,'y':1,'n':0,'s':1,'e':0,'w':0},{'x':7,'y':2,'n':0,'s':0,'e':0,'w':1},{'x':7,'y':3,'n':1,'s':1,'e':0,'w':0},{'x':7,'y':4,'n':1,'s':0,'e':0,'w':0},{'x':7,'y':5,'n':0,'s':1,'e':1,'w':0},{'x':7,'y':6,'n':1,'s':0,'e':1,'w':1},{'x':7,'y':7,'n':1,'s':0,'e':0,'w':0}]";
    // Handle logic here, or in motor code
    // Expected input example: "{{x:1,y:1,n:1,s:1,e:0,w:0},{x:2,y:2,n:0,s:1,e:0,w:1},{x:0,y:5,n:0,s:1,e:1,w:1}}"
    printed = snprintf(pcInsert, iInsertLen, "[{'x':0,'y':0,'n':1,'s':0,'e':1,'w':1},{'x': 0, 'y': 1, 'n': 1, 's': 0, 'e': 1, 'w': 1}, {'x': 0, 'y': 2, 'n': 0, 's': 1, 'e': 1, 'w': 1}, {'x': 0, 'y': 3, 'n': 1, 's': 1, 'e': 1, 'w': 0}, {'x': 0, 'y': 4, 'n': 0, 's': 1, 'e': 1, 'w': 0}, {'x': 0, 'y': 5, 'n': 1, 's': 0, 'e': 1, 'w': 0}, {'x': 0, 'y': 6, 'n': 1, 's': 0, 'e': 0, 'w': 1}, {'x': 0, 'y': 7, 'n': 1, 's': 0, 'e': 0, 'w': 0}]");
  }
  break;
  default:
    printed = 0;
    break;
  }

  return (u16_t)printed;
}

void ssi_init()
{
  // Initialise ADC (internal pin)
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);

  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}
