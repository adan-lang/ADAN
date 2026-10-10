#!/usr/bin/env bash
find ./src -type f \( -name '*.cc' -o -name '*.hh' \) -print0 |
	xargs -0 wc -l |
	awk '
		$2 != "total" { lines += $1 }
		function format_number(n, s, result) {
			s = sprintf("%.0f", n)
			result = ""
			while (length(s) > 3) {
				result = "," substr(s, length(s) - 2) result
				s = substr(s, 1, length(s) - 3)
			}
			return s result
		}
		END { print format_number(lines + 0) }
	'