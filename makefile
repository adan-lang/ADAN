.SILENT:

.PHONY: test push

test:
	@cargo b
	@./target/debug/adan