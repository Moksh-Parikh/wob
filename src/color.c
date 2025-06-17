#define WOB_FILE "color.c"

#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"

struct wob_color
wob_color_from_argb8888(uint32_t argb)
{
	struct wob_color result = {
		.a = (float) (argb >> 24 & 0xFF) / 255.0f,
		.r = (float) (argb >> 16 & 0xFF) / 255.0f,
		.g = (float) (argb >> 8 & 0xFF) / 255.0f,
		.b = (float) (argb & 0xFF) / 255.0f,
	};

	return result;
}

struct wob_color
wob_color_blend_premultiplied(struct wob_color foreground, struct wob_color background)
{
	struct wob_color result = {
		.a = foreground.a + background.a * (1 - foreground.a),
		.r = foreground.r + background.r * (1 - foreground.a),
		.b = foreground.b + background.b * (1 - foreground.a),
		.g = foreground.g + background.g * (1 - foreground.a),
	};

	return result;
}

uint32_t
wob_color_to_argb(const struct wob_color color)
{
	uint8_t alpha = (uint8_t) (color.a * UINT8_MAX);
	uint8_t red = (uint8_t) (color.r * UINT8_MAX);
	uint8_t green = (uint8_t) (color.g * UINT8_MAX);
	uint8_t blue = (uint8_t) (color.b * UINT8_MAX);

	return (alpha << 24) + (red << 16) + (green << 8) + blue;
}

uint32_t
wob_color_to_rgba(const struct wob_color color)
{
	uint8_t alpha = (uint8_t) (color.a * UINT8_MAX);
	uint8_t red = (uint8_t) (color.r * UINT8_MAX);
	uint8_t green = (uint8_t) (color.g * UINT8_MAX);
	uint8_t blue = (uint8_t) (color.b * UINT8_MAX);

	return (red << 24) + (green << 16) + (blue << 8) + alpha;
}

struct wob_color
wob_color_premultiply_alpha(const struct wob_color color)
{
	struct wob_color premultiplied_color = {
		.a = color.a,
		.r = color.r * color.a,
		.g = color.g * color.a,
		.b = color.b * color.a,
	};

	return premultiplied_color;
}

int
hex_to_int(char c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	if (c >= 'a' && c <= 'f') {
		return c - 'a' + 10;
	}
	if (c >= 'A' && c <= 'F') {
		return c - 'A' + 10;
	}

	return -1;
}

bool
wob_color_from_rgba_string(const char *str, struct wob_color *color)
{
	unsigned long length = strlen(str);

	uint8_t parts[4];
	parts[3] = 0xFF;
	switch (length) {
		case 8:
			int p6 = hex_to_int(str[6]);
			int p7 = hex_to_int(str[7]);

			if (p6 < 0 || p7 < 0) {
				return false;
			}

			parts[3] = p6 * 16 + p7;
			// fallthrough
		case 6:
			int p0 = hex_to_int(str[0]);
			int p1 = hex_to_int(str[1]);
			int p2 = hex_to_int(str[2]);
			int p3 = hex_to_int(str[3]);
			int p4 = hex_to_int(str[4]);
			int p5 = hex_to_int(str[5]);

			if (p0 < 0 || p1 < 0 || p2 < 0 || p3 < 0 || p4 < 0 || p5 < 0) {
				return false;
			}

			parts[0] = p0 * 16 + p1;
			parts[1] = p2 * 16 + p3;
			parts[2] = p4 * 16 + p5;

			break;
		default:
			return false;
	}

	*color = (struct wob_color) {
		.r = (float) parts[0] / UINT8_MAX,
		.g = (float) parts[1] / UINT8_MAX,
		.b = (float) parts[2] / UINT8_MAX,
		.a = (float) parts[3] / UINT8_MAX,
	};

	return true;
}
