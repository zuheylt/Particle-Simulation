#include <math.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include <time.h>

#define RADIUS  3
#define NUM_PARTICLES 28360                                                          
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define GAP_X (SCREEN_WIDTH/NUM_GRID_X)
#define GAP_Y (SCREEN_HEIGHT/NUM_GRID_Y)
#define NUM_GRID_X 8
#define NUM_GRID_Y 3
#define GRID_NUMBER (NUM_GRID_X * NUM_GRID_Y)

const int BASE_PARTICLES_PER_GRID = NUM_PARTICLES/GRID_NUMBER;
const int REMAINDER_PARTICLES = NUM_PARTICLES % GRID_NUMBER;
const int MAX_INITIAL_SPEED = 500;
typedef struct {
    float x, y;
    float vx, vy;
    float ax, ay;
}Particle;

typedef struct {
    int x_origin;
    int y_origin;
    Particle *particles;

}Grid;


float distance(Particle p1, Particle p2){
    return sqrtf((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}
void initiate_grids(Grid grids[]) {    
    for(int l = 0; l < NUM_GRID_Y; l++) {
        for (int c = 0; c < NUM_GRID_X; c++) {
            int grid_index = l* NUM_GRID_X + c;

            int particles_in_this_grid = BASE_PARTICLES_PER_GRID + (grid_index < REMAINDER_PARTICLES ? 1:0);
            
            grids[grid_index].x_origin = GAP_X * c;
            grids[grid_index].y_origin = GAP_Y * l;
                
            grids[grid_index].particles = malloc(particles_in_this_grid*sizeof(Particle));
            
            if (!grids[grid_index].particles) {
                fprintf(stderr, "Memory allocation failed for grid[%d].particles\n", grid_index);
                
                exit(EXIT_FAILURE);
            }
        }
    }
}                                                                                                                       
void free_grids(Grid grids[]) {
    for (int i = 0; i < GRID_NUMBER; i++) {
        free(grids[i].particles);  
    }
}
void initiate_particles(Particle particles[], Grid grids[]){
   int particle_index =0;


   for (int grid_idx=0; grid_idx <  GRID_NUMBER; grid_idx++){                                                                                                        
        int particles_in_this_grid = BASE_PARTICLES_PER_GRID + (grid_idx < REMAINDER_PARTICLES ? 1 : 0 );
        
        
        
        for (int j = 0; j < particles_in_this_grid; j++){

             int valid;
            do{
                valid=1;
                grids[grid_idx].particles[j].x = rand()%(GAP_X-(RADIUS)) + RADIUS + grids[grid_idx].x_origin;
                grids[grid_idx].particles[j].y = rand()%(GAP_Y-(RADIUS)) + RADIUS + grids[grid_idx].y_origin;
            

                
                for (int k = 0; k<j; k++){
                    if (distance(grids[grid_idx].particles[j],grids[grid_idx].particles[k])< (2*RADIUS+1)){
                        valid = 0;
                        break;
                    }
                }
            }while(!valid);
            grids[grid_idx].particles[j].vx=((float)rand()/RAND_MAX) * MAX_INITIAL_SPEED - (MAX_INITIAL_SPEED/2);
            grids[grid_idx].particles[j].vy=((float)rand()/RAND_MAX) * MAX_INITIAL_SPEED - (MAX_INITIAL_SPEED/2);
            grids[grid_idx].particles[j].ax=0;
            grids[grid_idx].particles[j].ay=0;
            particles[particle_index++]=grids[grid_idx].particles[j];
            

        }

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
    printf("Starting simulator \n");
    
    srand((unsigned int)time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO)!= 0){
        printf("SDL couldn't initialized SDL_ERROR: %s \n",SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Static Particle Simulation", 
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            SCREEN_WIDTH,
                                            SCREEN_HEIGHT,
                                            SDL_WINDOW_FULLSCREEN);
    if (!window){
        fprintf(stderr,"window couldn't created SDL_ERROR: %s \n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
    fprintf(stderr, "Renderer couldn't be created. SDL_ERROR: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
    }

    if (TTF_Init() != 0){
        printf("TTF_Init:%s", TTF_GetError());
        return 1;
    }
    TTF_Font *font = TTF_OpenFont("/home/temel/Documents/GitHub/Particle-Simulation/arial.ttf",20);
    
    if (!font) {
        fprintf(stderr, "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Color textColor = {0, 128, 0, 255};

    SDL_Event event;

    Particle particles[NUM_PARTICLES];
    Grid grids[GRID_NUMBER];
    printf("initiating grids\n");
    initiate_grids(grids);
    printf("initiating particles\n");
    initiate_particles(particles,grids);
    printf("particles initiated\n");


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

        SDL_SetRenderDrawColor(renderer,255,200,50,255);
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
        Uint32 frame_time = (Uint32)((frame_end - frame_start) * 1000 / frequency);
        frame_count++;
        if(frame_time<16){
            SDL_Delay(16-frame_time);
        }

        Uint64 currentTime = SDL_GetPerformanceCounter();
        double passed_time = (currentTime-start_time)/(double)frequency;
        if(passed_time >= 1.0){
            snprintf(fps_text, sizeof(fps_text), "FPS: %d", frame_count);
            start_time = SDL_GetPerformanceCounter();
            frame_count=0;
            
        }   
         

        


    }  

    free_grids(grids);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
    return 0;
}
