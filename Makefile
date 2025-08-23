.PHONY: help
help:
	@echo "Usage: make [build|upload|monitor]"

.PHONY: build
build:
	pio run

.PHONY: upload
upload:
	pio run --target upload

.PHONY: monitor
monitor:
	pio device monitor

