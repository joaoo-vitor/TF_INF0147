#version 330 core

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

// Atributos de fragmentos recebidos como entrada ("in") pelo Fragment Shader.
// Neste exemplo, este atributo foi gerado pelo rasterizador como a
// interpolação da posição global e a normal de cada vértice, definidas em
// "shader_vertex.glsl" e "main.cpp".
in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;
in vec4 normal_modelspace;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define PLANE 0
#define SKYBOX 1
#define CAR_BODY 2
#define CAR_PLAQUES 3
#define CAR_TYRES 4
#define CAR_GLASSES 5

uniform int object_id;

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

uniform sampler2D TextureImage0;
uniform sampler2D TextureImage1;
uniform sampler2D TextureImage2;

// O valor de saída ("out") de um Fragment Shader é a cor final do fragmento.
out vec4 color;

#define LIGHT_MODEL_BLINNPHONG 0
#define LIGHT_MODEL_LAMBERT 1
#define LIGHT_MODEL_NO_MODEL 2

int light_model;

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define o
    // sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    // O fragmento atual é coberto por um ponto que percente à superfície de um
    // dos objetos virtuais da cena. Este ponto, p, possui uma posição no
    // sistema de coordenadas global (World coordinates). Esta posição é obtida
    // através da interpolação, feita pelo rasterizador, da posição de cada
    // vértice.
    vec4 p = position_world;

    // Normal do fragmento atual, interpolada pelo rasterizador a partir das
    // normais de cada vértice.
    vec4 n = normalize(normal);
    
   // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(1.0,1.0,0.0,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2*n*(dot(n, l)); // PREENCHA AQUI o vetor de reflexão especular ideal

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd = vec3(0.0,0.0,0.0);; // Refletância difusa (sempre utilizada)
    vec3 Ks; // Refletância especular (nem sempre calculada)
    vec3 Ka = vec3(0.1,0.1,0.1); // Refletância ambiente 
    //(minima para não ter lugares em preto, evitando baixo fotorealismo)

    float q; // Expoente especular para o modelo de iluminação de Phong

    // Coordenadas de textura U e V
    float U = 0.0;
    float V = 0.0;

    if ( object_id == CAR_BODY )
    {   
        // Propriedades espectrais do carro
        light_model=LIGHT_MODEL_LAMBERT;

        vec4 nm = normalize(normal_modelspace);

        // Projeção cúbica para o carro
        if (abs(dot(nm, vec4(1.0, 0.0, 0.0, 0.0))) > abs(dot(nm, vec4(0.0, 1.0, 0.0, 0.0)))){
            if (abs(dot(nm, vec4(1.0, 0.0, 0.0, 0.0))) > abs(dot(nm, vec4(0.0, 0.0, 1.0, 0.0)))){
                // Projeção com plano YZ
                U = (position_model.y - bbox_min.y) / (bbox_max.y - bbox_min.y);
                V = (position_model.z - bbox_min.z) / (bbox_max.z - bbox_min.z);
            }else{
                // Projeção com eixo XY
                U = (position_model.x - bbox_min.x) / (bbox_max.x - bbox_min.x);
                V = (position_model.y - bbox_min.y) / (bbox_max.y- bbox_min.y);
            }
        }else{
            if (abs(dot(nm, vec4(0.0, 1.0, 0.0, 0.0))) > abs(dot(nm, vec4(0.0, 0.0, 1.0, 0.0)))){
                // Projeção com eixo XZ
                U = (position_model.x - bbox_min.x) / (bbox_max.x - bbox_min.x);
                V = (position_model.z - bbox_min.z) / (bbox_max.z - bbox_min.z);
            }else{
                // Projeção com eixo XY
                U = (position_model.x - bbox_min.x) / (bbox_max.x - bbox_min.x);
                V = (position_model.y - bbox_min.y) / (bbox_max.y - bbox_min.y);
            }
        }
        Kd = texture(TextureImage2, vec2(U,V)).rgb;

    }else if (object_id == CAR_GLASSES){
        light_model=LIGHT_MODEL_BLINNPHONG;
        Kd= vec3(0.7, 0.7, 0.7);
        Ks = vec3(0.9, 0.9, 0.1);
        q=90;
    }
    else if (object_id == PLANE)
    {
        // Propriedades espectrais do plano 
        light_model=LIGHT_MODEL_LAMBERT;

        // Projeção planar
        U = position_world.x * 0.002f;
        V = position_world.z * 0.002f;
        Kd = texture(TextureImage0, vec2(U,V)).rgb;
        Ks = vec3(0.0, 0.0, 0.0);
    } else if(object_id == SKYBOX){
        light_model=LIGHT_MODEL_NO_MODEL;

        vec3 bbox_center = ((bbox_min + bbox_max) / 2.0).xyz;
        vec3 p_linha = normalize(position_model.xyz - bbox_center);

        //if (p_linha.y < 0) return;
        // Convert direction into spherical coordinates
        float U = atan(p_linha.z, p_linha.x) / (2.0 * M_PI) + 0.5;
        float V = asin(p_linha.y);  // simple: height maps directly to y
        Kd = texture(TextureImage1, vec2(U,V)).rgb;
    }
    else // Objeto desconhecido = cor default, cinza escuro
    {
        light_model=LIGHT_MODEL_NO_MODEL;
        Kd = vec3(0.05,0.05,0.05);
    }

    // Define o modelo de iluminação a partir do tipo
    if(light_model == LIGHT_MODEL_BLINNPHONG){
        // TODO - modelo blin phong
        vec3 I = vec3(1.0,1.0,1.0); // O espectro da fonte de luz
        vec3 Ia = vec3(0.13,0.13,0.13); // O espectro da luz ambiente
        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l)); // o termo difuso de Lambert
        vec3 ambient_term = Ka * Ia; // O termo ambiente

        // Blinn-Phong uses half vector
        vec4 h = normalize(l+v);
        vec3 phong_specular_term  = Ks * I * pow(max(dot(n, h), 0.0), q); // o termo especular de Phong
        color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

    }else if (light_model == LIGHT_MODEL_LAMBERT) {
        vec3 I = vec3(1.0,1.0,1.0); // O espectro da fonte de luz
        vec3 Ia = vec3(0.2,0.2,0.2); // O espectro da luz ambiente
        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l)); // PREENCHA AQUI o termo difuso de Lambert
        vec3 ambient_term = Ka * Ia; // O termo ambiente
        color.rgb = lambert_diffuse_term + ambient_term;
    } else {
        color.rgb = Kd;
    }

    color.a = 1;
    // Cor final com correção gamma, considerando monitor sRGB.
    // Veja https://en.wikipedia.org/w/index.php?title=Gamma_correction&oldid=751281772#Windows.2C_Mac.2C_sRGB_and_TV.2Fvideo_standard_gammas
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
}

