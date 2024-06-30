#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

uint8_t *read_entire_file(const char *file_path, size_t *file_len)
{
	FILE *f = fopen(file_path, "rb");
	int64_t m;
	uint8_t *buffer;

	if (f == NULL) goto error;
	if (fseek(f, 0, SEEK_END) < 0) goto error;

	m = ftell(f);
	if (m < 0) goto error;

	buffer = malloc((size_t) m);
	if (buffer == NULL) goto error;
	if (fseek(f, 0, SEEK_SET) < 0) goto error;
	*file_len = fread(buffer, 1, (size_t) m, f);
	assert(*file_len == (size_t) m);

	if (ferror(f)) goto error;
	fclose(f);
	return buffer;

 error:
	if (f) fclose(f);
	if (buffer) free(buffer);
	return NULL;
}

uint32_t base64_from_integer(uint32_t value, uint32_t bytes_cnt)
{
	static const char *TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	uint32_t base64_value;
	base64_value = 0;
	if (bytes_cnt > 2) base64_value = (base64_value << 8) | TABLE[(value >> (6 * 0)) & 0x3F];
	else               base64_value = (base64_value << 8) | '=';
	if (bytes_cnt > 1) base64_value = (base64_value << 8) | TABLE[(value >> (6 * 1)) & 0x3F];
	else               base64_value = (base64_value << 8) | '=';

	base64_value = (base64_value << 8) | TABLE[(value >> (6 * 2)) & 0x3F];
	base64_value = (base64_value << 8) | TABLE[(value >> (6 * 3)) & 0x3F];

	return base64_value;
}

uint8_t *encode_base64(const uint8_t *content, size_t content_len, size_t *digest_len)
{
	uint8_t *digest;
	size_t i, j, cnt, d_len;
	uint32_t *dp32, value, bytes_cnt;
	uint8_t *dp8;

	d_len = content_len / 3;
	if (content_len % 3) d_len++;
	d_len *= 4;
	d_len += 2* (d_len/76);
	digest = malloc(d_len);
	assert(digest != NULL);

	dp32 = (uint32_t*) digest;
	dp8 = digest;
	cnt = 0;
	for (i = 0; i < content_len; i += 3) {
		bytes_cnt = 0;
		value = 0;
		for (j = i; j < content_len  && j < i + 3; j++) {
			bytes_cnt++;
			value |= (content[j] << ((3-bytes_cnt) * 8));
		}
		*dp32++ = base64_from_integer(value, bytes_cnt);
		dp8 += 4;
		cnt += 4;
		if (cnt % 76 == 0) {
			*dp8++ = '\r';
			*dp8++ = '\n';
			dp32 = (uint32_t*) dp8;
		}
	}
	*digest_len = d_len;
	return digest;
}

int main()
{
	const char *path = "teste.txt";
	size_t content_len, digest_len, i;
	const uint8_t *content = read_entire_file(path, &content_len);

	if (content == NULL) {
		fprintf(stderr, "Error: Could not open file %s\n", path);
		exit(1);
	}
	uint8_t *digest = encode_base64(content, content_len, &digest_len);

	for (i = 0; i < digest_len; i++) fprintf(stdout, "%c", digest[i]);
	fprintf(stdout, "\n");

	free(digest);

	return 0;
}
