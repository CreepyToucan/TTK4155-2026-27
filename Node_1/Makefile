SOURCE_FILES := src/main.c Drivers/UART.c Drivers/XMEM.c Drivers/ADC.c Libraries/Analogs.c Drivers/SPI.c Libraries/Oled.c Libraries/io_board.c

# Set this flag to "yes" (no quotes) to use JTAG; otherwise ISP (SPI) is used
PROGRAM_WITH_JTAG := yes

# Feel free to ignore anything below this line
PROGRAMMER := atmelice_isp
ifeq ($(PROGRAM_WITH_JTAG), yes)
	PROGRAMMER := atmelice
endif

BUILD_DIR := build
TARGET_CPU := atmega162
TARGET_DEVICE := m162

CC := avr-gcc

# ==========================================
# NUOVA SEZIONE PER LA GESTIONE DELLE CARTELLE
# ==========================================

# 1. Ricava la lista delle cartelle dai percorsi dei file sorgente (rimuovendo i duplicati con 'sort')
SRC_DIRS := $(sort $(dir $(SOURCE_FILES)))

# 2. Crea i flag -I per permettere a GCC di trovare i file .h nelle stesse cartelle
INCLUDE_FLAGS := $(foreach dir, $(SRC_DIRS), -I$(dir))

# 3. Aggiungi i flag di include ai CFLAGS esistenti
CFLAGS := -O -std=c11 -mmcu=$(TARGET_CPU) -ggdb $(INCLUDE_FLAGS)

# 4. Usa 'vpath' per dire a Make in quali cartelle cercare i file .c quando deve compilare
vpath %.c $(SRC_DIRS)

# 5. Appiattisci i file oggetto: rimuove il percorso (notdir) e mette tutto in build/
# Esempio: "src/UART.c" diventa "build/UART.o"
OBJECT_FILES := $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCE_FILES:.c=.o)))

# ==========================================

.DEFAULT_GOAL := $(BUILD_DIR)/main.hex

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.hex: $(OBJECT_FILES) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJECT_FILES) -o $(BUILD_DIR)/a.out
	avr-objcopy -j .text -j .data -O ihex $(BUILD_DIR)/a.out $(BUILD_DIR)/main.hex

.PHONY: flash
flash: $(BUILD_DIR)/main.hex
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -U flash:w:$(BUILD_DIR)/main.hex:i

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: erase
erase:
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -e
	
.PHONY: debug
debug:
	if pgrep avarice; then pkill avarice; fi
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -U flash:w:$(BUILD_DIR)/main.hex:i
	x-terminal-emulator -e avarice --edbg --ignore-intr :4242
	sleep 2
	avr-gdb -tui -iex "target remote localhost:4242" $(BUILD_DIR)/a.out
	killall -s 9 avarice
