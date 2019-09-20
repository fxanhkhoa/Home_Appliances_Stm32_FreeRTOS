#include "dht22.h"

GPIO_InitTypeDef GPIO_InitStruct = {0};

void DHT22_Init(void)
{
	DHT22_GPIO_CLOCK; //__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = DHT22_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(DHT22_GPIO_PORT, &GPIO_InitStruct);
}

static uint8_t DHT22_GetReadings(dht22_data *out)
{
	uint16_t c;
	uint8_t i;
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DHT22_GPIO_PORT, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(DHT22_GPIO_PORT, DHT22_GPIO_PIN, GPIO_PIN_RESET);
//	 DHT22_GPIO_PORT->BRR = DHT22_GPIO_PIN; // Pull down SDA (Bit_SET)
	// Delay 2ms
	Delay_us(900);
	HAL_GPIO_WritePin(DHT22_GPIO_PORT, DHT22_GPIO_PIN, GPIO_PIN_SET);
//	 DHT22_GPIO_PORT->BSRR = DHT22_GPIO_PIN; // Release SDA (Bit_RESET)
	
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DHT22_GPIO_PORT, &GPIO_InitStruct);
	
	// Wait for AM2302 to begin communication (20-40us)
  DELAY_US_TIM->CNT = 0;
  while (((c = DELAY_US_TIM->CNT) < 100)
      && (HAL_GPIO_ReadPin(DHT22_GPIO_PORT, DHT22_GPIO_PIN)));
	
	DELAY_US_TIM->CNT = 0;

  if (c >= 100)
    return DHT22_RCV_NO_RESPONSE;
	
	// Check ACK strobe from sensor
  while (((c = DELAY_US_TIM->CNT) < 100)
      && !(HAL_GPIO_ReadPin(DHT22_GPIO_PORT, DHT22_GPIO_PIN)));

  DELAY_US_TIM->CNT = 0;
	
	if ((c < 65) || (c > 95))
    return DHT22_RCV_BAD_ACK1;
	
	while (((c = DELAY_US_TIM->CNT) < 100)
      && (HAL_GPIO_ReadPin(DHT22_GPIO_PORT, DHT22_GPIO_PIN))){};

  DELAY_US_TIM->CNT = 0;

  if ((c < 65) || (c > 95))
    return DHT22_RCV_BAD_ACK2;
	
	// ACK strobe received --> receive 40 bits
  i = 0;
	
	while (i < 40)
	{
		// Measure bit start impulse (T_low = 50us)

		while (((c = DELAY_US_TIM->CNT) < 100)
	  && !(HAL_GPIO_ReadPin(DHT22_GPIO_PORT, DHT22_GPIO_PIN))){};
			
		 if (c > 75)
		{
			// invalid bit start impulse length
			out->bits[i] = 0xff;
			while (((c = DELAY_US_TIM->CNT) < 100)
					&& (HAL_GPIO_ReadPin(DHT22_GPIO_PORT, DHT22_GPIO_PIN)))
				;
			DELAY_US_TIM->CNT = 0;
		}
		else
		{
			// Measure bit impulse length (T_h0 = 25us, T_h1 = 70us)
			DELAY_US_TIM->CNT = 0;
			while (((c = DELAY_US_TIM->CNT) < 100)
					&& (HAL_GPIO_ReadPin(DHT22_GPIO_PORT, DHT22_GPIO_PIN)))
				;
			DELAY_US_TIM->CNT = 0;
			out->bits[i] = (c < 100) ? (uint8_t) c : 0xff;
		}

      i++;
	}
	
	for (i = 0; i < 40; i++)
		if (out->bits[i] == 0xff)
			return DHT22_RCV_TIMEOUT;

	return DHT22_RCV_OK;
}

static void DHT22_DecodeReadings (dht22_data *out)
{
  uint8_t i = 0;

  for (; i < 8; i++)
    {
      out->hMSB <<= 1;
      if (out->bits[i] > 48)
	out->hMSB |= 1;
    }

  for (; i < 16; i++)
    {
      out->hLSB <<= 1;
      if (out->bits[i] > 48)
	out->hLSB |= 1;
    }

  for (; i < 24; i++)
    {
      out->tMSB <<= 1;
      if (out->bits[i] > 48)
	out->tMSB |= 1;
    }

  for (; i < 32; i++)
    {
      out->tLSB <<= 1;
      if (out->bits[i] > 48)
	out->tLSB |= 1;
    }

  for (; i < 40; i++)
    {
      out->parity_rcv <<= 1;
      if (out->bits[i] > 48)
	out->parity_rcv |= 1;
    }

  out->parity = out->hMSB + out->hLSB + out->tMSB + out->tLSB;

}

static uint16_t DHT22_GetHumidity (dht22_data *out)
{
  return (out->hMSB << 8) | out->hLSB;
}

static uint16_t DHT22_GetTemperature (dht22_data *out)
{
  return (out->tMSB << 8) | out->tLSB;
}

int DHT22_Read (dht22_data *out)
{
  out->rcv_response = DHT22_GetReadings (out);

  if (out->rcv_response != DHT22_RCV_OK)
	{
		return 0;
	}

  DHT22_DecodeReadings (out);

  if (out->parity != out->parity_rcv)
	{
		out->rcv_response = DHT22_BAD_DATA;
		return 0;
	}

  out->humidity = (float) DHT22_GetHumidity (out) / 10.0f;

  uint16_t temperature = DHT22_GetTemperature (out);
  out->temperature = ((float) (temperature & 0x7fff)) / 10.0f;

  if (temperature & 0x8000)
	{
		out->temperature = -out->temperature;
	}

  return 1;

}
