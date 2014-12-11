// HX8340B 2.2" TFT screen
// Pin configuration
// MOSI: P2.0
// MISO: P2.1
// SCK: P2.2

// SPI mode 0
// CPOL 0, CPHA 1

void SPI_init()
{

}


void SPI_write(unsigned short c)
{
	data = c & 1
	sck = 1
	sck = 0
	data = (c >> 1) & 1
	sck = 1

	sck = 0
}