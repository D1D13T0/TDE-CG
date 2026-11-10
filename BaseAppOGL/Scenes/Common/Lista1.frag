#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D Texture0;
uniform vec4  uColor;        // Cor do objeto (multiplica a textura)
uniform bool  uUseTexture;   // Usa a textura?
uniform bool  uUseLighting;  // Usa iluminacao?
uniform bool  uTwoSided;     // Ilumina as duas faces (paredes/malhas abertas)
uniform vec3  uLightDir;     // Direcao (do fragmento para a luz)
uniform vec3  uViewPos;      // Posicao da camera
uniform float uAmbient;      // Intensidade ambiente (0..1)
uniform float uSpecular;     // Intensidade especular

// Luz pontual colorida opcional (ex.: luz rosa do chao no Exercicio 1)
uniform bool  uPointOn;
uniform vec3  uPointPos;
uniform vec3  uPointColor;   // Cor que tinge os objetos proximos
uniform float uPointRadius;  // Alcance da luz
uniform float uPointIntensity;

// Degrade de cor no espaco do mundo (chao do Exercicio 1: branco ao fundo, rosa perto da camera)
uniform bool  uGradOn;
uniform vec3  uGradFar;      // Cor no fundo
uniform vec3  uGradNearL;    // Cor na frente, lado esquerdo
uniform vec3  uGradNearR;    // Cor na frente, lado direito
uniform float uGradZFar;     // z onde o degrade comeca
uniform float uGradZNear;    // z onde o degrade termina
uniform float uGradXHalf;    // Meia largura usada na variacao esquerda/direita

// Reflexos coloridos suaves conforme a direcao da normal (neve do boneco)
uniform bool  uTintOn;

void main()
{
    vec4 base = uColor;
    if (uUseTexture)
        base *= texture(Texture0, TexCoord);

    if (uGradOn)
    {
        float t = clamp((FragPos.z - uGradZFar) / (uGradZNear - uGradZFar), 0.0, 1.0);
        float k = clamp(FragPos.x / (2.0 * uGradXHalf) + 0.5, 0.0, 1.0);
        base.rgb *= mix(uGradFar, mix(uGradNearL, uGradNearR, k), t);
    }

    if (!uUseLighting)
    {
        FragColor = base;
        return;
    }

    vec3 N = normalize(Normal);
    vec3 L = normalize(uLightDir);
    vec3 V = normalize(uViewPos - FragPos);

    if (uTwoSided && dot(N, V) < 0.0)
        N = -N;

    float diff = max(dot(N, L), 0.0);
    vec3  R    = reflect(-L, N);
    float spec = pow(max(dot(V, R), 0.0), 32.0) * uSpecular;

    vec3 color = base.rgb * (uAmbient + (1.0 - uAmbient) * diff) + vec3(spec);

    if (uTintOn)
    {
        vec3 tint = vec3(1.0);
        tint = mix(tint, vec3(1.00, 0.97, 0.78), max(-N.x, 0.0) * 0.8);   // amarelado a esquerda
        tint = mix(tint, vec3(0.80, 1.00, 0.97), max( N.x, 0.0) * 0.8);   // azulado a direita
        tint = mix(tint, vec3(1.00, 0.80, 0.95), max(-N.y, 0.0) * 0.9);   // rosado embaixo (reflexo do chao)
        color *= tint;
    }

    if (uPointOn)
    {
        vec3  toLight = uPointPos - FragPos;
        float dist    = length(toLight);
        float atten   = clamp(1.0 - dist / uPointRadius, 0.0, 1.0);
        atten *= atten;
        float nDotL   = max(dot(N, toLight / dist), 0.0);
        float f       = clamp(atten * nDotL * uPointIntensity, 0.0, 1.0);
        color *= mix(vec3(1.0), uPointColor, f);
    }
    FragColor = vec4(color, base.a);
}
