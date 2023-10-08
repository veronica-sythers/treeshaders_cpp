shader_type spatial;
render_mode blend_mix;

const int NUM_VIGNETTES = 5;
uniform float vignette_radius = 0.1;
uniform vec2 vignette_centers[NUM_VIGNETTES];

uniform bool wire_on = false;

uniform vec4 albedo : source_color;
uniform sampler2D image_texture;
uniform float alpha_threshold = 0.01;

uniform vec2 noise_speed = vec2(0.25, 0.0);
uniform float noise_scale = 0.06;

uniform float sway_frequency: hint_range(0.2, 2) = 1.0;
uniform float sway_amplitude: hint_range(0.01, 0.1) = 0.05;

uniform sampler2D noise_texture;
uniform sampler2D gradient_texture;
uniform float rotation : hint_range(-3.14159, 3.14159) = 0.0;

uniform float time_factor : hint_range(0.1, 3) = 1;
uniform float time_offset : hint_range(0.8, 1.2) = 1.0;


vec2 rotate(vec2 v, float a) {
    float s = sin(a);
    float c = cos(a);
    return vec2(c * v.x - s * v.y, s * v.x + c * v.y);
}

vec2 scaleUV(vec2 uv) {
    float scale_factor = 1.414;
    return 0.5 + (uv - vec2(0.5, 0.5)) / scale_factor;
}

vec3 make_turbulence(vec2 uv, float gradient_value) {
	vec2 ns = uv + noise_speed * (TIME + time_offset) * time_factor;
	float noise_value = texture(noise_texture, ns).r * 2.0 - 1.0;
    return vec3(
		noise_value * gradient_value * noise_scale,
		noise_value * gradient_value * noise_scale,
		0
	);
}

vec3 make_sway() {
    float sway_value = sin((TIME + time_offset) * sway_frequency) * sway_amplitude;
    return vec3(
		sin(-rotation + 3.14 / 2.0) * sway_value,
		cos(-rotation + 3.14 / 2.0) * sway_value,
		0
	);
}

vec3 perm_sway() {
    return vec3(
		sin(-rotation + 3.14 / 2.0) * sway_amplitude / 1.0,
		cos(-rotation + 3.14 / 2.0) * sway_amplitude / 1.0,
		0
	);	
}

void vertex() {
	vec2 rotatedUV = rotate(UV - vec2(0.5, 0.5), rotation) + vec2(0.5, 0.5);
	vec2 scaledUV = scaleUV(rotatedUV);
	float gradient_value = texture(gradient_texture, scaledUV).r;
	
	VERTEX += make_turbulence(scaledUV, gradient_value) + make_sway() - perm_sway();
}

//void fragment() {
//	vec2 uv = (vec2(0.0, 0.0) / vec2(1024.0, 1024.0)) + UV;
//	vec2 r = mod(vec2(1024.0, 1024.0) * uv, vec2(32));
//	vec2 lines = smoothstep(1.0 - 0.1, 1, r / vec2(32));
//	float alpha = dot(lines, vec2(1.0, 1.0));
//	ALPHA = alpha;
//}

void fragment() {
	vec3 sw = make_sway() - perm_sway();
	vec2 s2 = vec2(-sw.x, sw.y);
	
    float combined_vignette = 1.0;
    for (int i = 0; i < NUM_VIGNETTES; i++) {
		float dist = length(fract(UV) - vignette_centers[i] - s2);
	 	float vignette = smoothstep(vignette_radius, vignette_radius + 0.4, dist);
		combined_vignette *= vignette;
	}

	vec4 albedo_tex = texture(image_texture, UV);
	vec4 vignetted = mix(vec4(0.0, 0.0, 0.0, 0.0), albedo_tex.rgba, combined_vignette);
	ALBEDO = vignetted.rgb;
	if (vignetted.a  * albedo_tex.a < alpha_threshold) {
		discard;
	}
}