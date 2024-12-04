#include <math.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include <time.h>

#define RADIUS  2
#define NUM_PARTICLES 20000
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

typedef struct {
    float x, y;
    float vx, vy;
    float ax, ay;
}Particle;

Particle particles[NUM_PARTICLES];

float distance(Particle p1, Particle p2){
    return sqrtf((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}


void initiate_particles(Particle particles[], int num_particles){
    for (int i= 0; i<num_particles; i++){
        int valid;
        do{
            valid=1;
            particles[i].x = rand()%(SCREEN_WIDTH-(2*RADIUS))+RADIUS;
            particles[i].y = rand()%(SCREEN_HEIGHT-(2*RADIUS))+RADIUS;
            

            //Should be optimized with big numbers unable to finish
            for (int j = 0; j<i; j++){
                if (distance(particles[i],particles[j])< (2*RADIUS+1)){
                    valid = 0;
                }
            }
        }while(!valid);
        particles[i].vx=((float)rand()/RAND_MAX)*500-250;
        particles[i].vy=((float)rand()/RAND_MAX)*500-250;
        particles[i].ax=0;
        particles[i].ay=0;

    }
}

void update_particles(Particle particles[], int num_particles, float dt){
    for (int i=0;i<num_particles;i++){
        if(particles[i].x<RADIUS||particles[i].x>SCREEN_WIDTH-RADIUS){
            particles[i].vx *= -1;

        }
        if(particles[i].y<RADIUS||particles[i].y>SCREEN_HEIGHT-RADIUS){
            particles[i].vy *= -1;

        }


        particles[i].x += particles[i].vx*dt;
        particles[i].y += particles[i].vy*dt;
    }
}

void draw_circle(SDL_Renderer *renderer, Particle particles[]){
    for (int i=0; i<NUM_PARTICLES; i++){
        for (int w=0; w< RADIUS*2;w++){
            for (int h=0; h<RADIUS*2;h++){                           
                int dx = RADIUS-w;
                int dy = RADIUS-h;
                if (dx*dx+dy*dy<= RADIUS*RADIUS){
                SDL_RenderDrawPoint(renderer, (int) particles[i].x+dx, (int) particles[i].y+dy);  
                }
            }
        }
    } 
}


int main(){
    srand(time(NULL));

    if (TTF_Init() != 0){
        printf("TTF_Init:%s", TTF_GetError());
        return 1;
    }
    TTF_Font *font = TTF_OpenFont("/home/temel/Documents/GitHub/Particle-Simulation/arial.ttf",20);
    SDL_Color textColor = {0, 128, 0, 255};

    if (SDL_Init(SDL_INIT_VIDEO)!= 0){
        printf("SDL couldn't initialized SDL_ERROR: %s",SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Static Particle Simulation", 
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            SCREEN_WIDTH,
                                            SCREEN_HEIGHT,
                                            SDL_WINDOW_FULLSCREEN);
    if (!window){
        printf("window couldn't created SDL_ERROR: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer){
        printf("Couldn't created window SDL_ERROR: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    } 
    SDL_Event event;

    Particle particles[NUM_PARTICLES];
    initiate_particles(particles,NUM_PARTICLES);


    int running =1;
    Uint64 start_time= SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    int frame_count =0;
    char fps_text[20];

    while(running){
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT){
                running =0;
            }
        }
        Uint32 frame_start = SDL_GetPerformanceCounter();
        update_particles(particles,NUM_PARTICLES,0.016);

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        draw_circle(renderer,particles);

        SDL_Surface *textSurface = TTF_RenderText_Solid(font, fps_text, textColor);
        if (textSurface) {
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {10, 10, textSurface->w, textSurface->h};
            SDL_FreeSurface(textSurface);

            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
            }
        
        SDL_RenderPresent(renderer);

        Uint32 frame_end = SDL_GetPerformanceCounter();
        double frame_time = (double)(frame_end - frame_start)/frequency;
        frame_count++;
        if(frame_time<0.016){
            SDL_Delay(16-frame_time*1000);
        }

        Uint64 currentTİme = SDL_GetPerformanceCounter();
        double passed_time = (currentTİme-start_time)/(double)frequency;
        if(passed_time >= 1.0){
            snprintf(fps_text, sizeof(fps_text), "FPS: %d", frame_count);
            start_time = SDL_GetPerformanceCounter();
            frame_count=0;
            
        }
         

        


    }  

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
    return 0;
}
