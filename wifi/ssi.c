#include "ssi.h"
#include "barcode.h"
#include "mapping.h"

const char *ssi_tags[] = {"volt", "temp", "led", "width", "height", "grid1", "grid2", "grid3", "grid4", "grid5", "bc"};
char formatted_map[5][192];

void format_maze()
{
  int counter = 0;
  int curr_grid = 0;
  int current_pos = 1;

  formatted_map[counter][0] = '[';

  for (int y = 0; y < 6; y++)
  {
    for (int x = 0; x < 4; x++)
    {
      formatted_map[counter][current_pos] = '{';
      current_pos++;
      current_pos += snprintf(&formatted_map[counter][current_pos], 191 - current_pos, "'x':%d,'y':%d,'w':[%d,%d,%d,%d],'v':%d",
                              x, y,
                              mazeGrid[x][y].northWall, mazeGrid[x][y].southWall,
                              mazeGrid[x][y].eastWall, mazeGrid[x][y].westWall,
                              mazeGrid[x][y].visited);
      if (mazeGrid[x][y].visited == 2)
      {
        current_pos += snprintf(&formatted_map[counter][current_pos], 191 - current_pos, ",'c':'%c'", barcode_char);
      }

      formatted_map[counter][current_pos++] = '}';

      if ((curr_grid + 1) % 5 == 0)
      {
        formatted_map[counter][current_pos++] = ']';
        formatted_map[counter][current_pos++] = '\0';
        counter++;
        formatted_map[counter][0] = '[';
        current_pos = 1;
      }
      else
      {
        formatted_map[counter][current_pos++] = ',';
      }

      curr_grid++;
    }
  }

  if (formatted_map[counter][current_pos - 1] == ',')
  {
    formatted_map[counter][current_pos - 1] = ']'; // replace the last comma with a closing bracket
    formatted_map[counter][current_pos] = '\0';    // null-terminate the string
  }
}

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen)
{
  size_t printed;
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
    printed = snprintf(pcInsert, iInsertLen, "%d", 4); // Import max width details
  }
  break;
  case 4: // maxheight
  {
    printed = snprintf(pcInsert, iInsertLen, "%d", 6); // Import max height details
  }
  break;
  case 5: // grid1
  {
    if (isMazeMapped)
    {

      printed = snprintf(pcInsert, iInsertLen, formatted_map[0]);
    }
    else
    {
      printed = snprintf(pcInsert, iInsertLen, "");
    }
  }
  break;
  case 6: // grid2
  {
    if (isMazeMapped)
    {

      printed = snprintf(pcInsert, iInsertLen, formatted_map[1]);
    }
    else
    {
      printed = snprintf(pcInsert, iInsertLen, "");
    }
  }
  break;
  case 7: // grid3
  {
    if (isMazeMapped)
    {

      printed = snprintf(pcInsert, iInsertLen, formatted_map[2]);
    }
    else
    {
      printed = snprintf(pcInsert, iInsertLen, "");
    }
  }
  break;
  case 8: // grid4
  {
    if (isMazeMapped)
    {

      printed = snprintf(pcInsert, iInsertLen, formatted_map[3]);
    }
    else
    {
      printed = snprintf(pcInsert, iInsertLen, "");
    }
  }
  break;
  case 9: // grid5
  {
    if (isMazeMapped)
    {

      printed = snprintf(pcInsert, iInsertLen, formatted_map[4]);
    }
    else
    {
      printed = snprintf(pcInsert, iInsertLen, "");
    }
  }
  break;
  case 10: // bc
  {
    printed = snprintf(pcInsert, iInsertLen, "%c", barcode_char);
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

  if (isMazeMapped)
  {
    format_maze();
  }
  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}
