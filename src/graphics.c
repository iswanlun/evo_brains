#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "graphics.h"
#include "gd.h"

#include <stdio.h>

#define IMG_SCALE 4

static FILE* gif_fp;
static const char* path;
static gdImage* im_p, *im_c;

static int tsp;
static int img_x_dim;
static int img_y_dim;

int graphics_init( const char* path_name ) {

    printf("g_init\n");

    path = path_name;
    struct stat st;

    return ( stat(path, &st) | !S_ISDIR(st.st_mode) );
}

void graphics_start_gif( environment* env, const char* fname ) {

    printf("g_start\n");

    /* open new file */
    char fpath[ strlen(path) + strlen(fname) ];
    sprintf( fpath, "%s%s_Gen_%d.gif", path, fname, env->gen );
    gif_fp = fopen( fpath, "wb" );

    img_x_dim = (env->x_dim * IMG_SCALE);
    img_y_dim = (env->y_dim * IMG_SCALE);

    im_p = gdImageCreate(img_x_dim, img_y_dim);
    (void)gdImageColorAllocate(im_p, 255, 255, 255); /* white background */
    tsp = gdImageColorAllocate(im_p, 1, 1, 1); /* allows transparent pixels */
    gdImageGifAnimBegin( im_p, gif_fp, 1, 0 );
    gdImageGifAnimAdd(im_p, gif_fp, 0, 0, 0, 0, 1, NULL);

    for ( int i = 0; i < env->population; ++i ) {

        uint32_t fgene = env->brains[i]->raw_genome[0];
        env->brains[i]->color = gdImageColorAllocate( im_p, ((fgene>>24) & 0xFF), ((fgene>>16) & 0xFF), ((fgene>>8) & 0xFF) );
    }

}

void graphics_add_frame( environment* env ) {

    printf("g_frame\n");

    /* Create and add new frame to gif */
    im_c = gdImageCreate(img_x_dim, img_y_dim);
    (void)gdImageColorAllocate(im_p, 255, 255, 255); /* white background */
    gdImagePaletteCopy(im_c, im_p);  // Make sure the palette is identical

    printf("preloop\n");

    for ( int i = 0; i < env->population; ++i ) {

        int x = (env->brains[i]->x_pos * IMG_SCALE);
        int y = (env->brains[i]->y_pos * IMG_SCALE);

        gdImageFilledRectangle(im_c, x, y, x+(IMG_SCALE-1), y+(IMG_SCALE-1), env->brains[i]->color );
    }

    printf("post loop\n");

    gdImageColorTransparent( im_c, tsp );

    printf("tsp fin.\n");

    gdImageGifAnimAdd(im_c, gif_fp, 0, 0, 0, 100, 1, im_p);

    printf(" im_c: %i  im_p: %i gif_fp: %i \n", (int)im_c, (int)im_p, (int)gif_fp );

    printf("added\n");

    gdImageDestroy(im_p);
    im_p = im_c;
    im_c = NULL;
}

void graphics_generate_gif( void ) {

    printf("img end\n");

    /* generate gif, dispose of all resources */
    gdImageGifAnimEnd( gif_fp );
    gdImageDestroy( im_p );
    fclose( gif_fp );
    
}