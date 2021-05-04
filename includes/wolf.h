/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wolf.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/24 15:31:21 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/04 16:17:14 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOLF_H
# define WOLF_H

//# include "/usr/local/include/mlx.h"
# include "C:\SDL\SDL2-2.0.12\include\SDL.h"
# include <CL/cl.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <math.h>
# include "libft.h"
# include "get_next_line.h"
# include "bmp.h"
# include <stdio.h>
# include "sync_data.h"

//GPU program compile directives

# define GPU_INCLUDES	"-I ./includes/ "

# ifdef	RENDER_VECTOR
#  define	GPU_ENTRY	"render_vectors"
# else
#  define	GPU_ENTRY	"render"
# endif

/*
** Party member info
** name		= party member name
** maxhp	= max HP
** hp		= current hp
** defend	= 1 if defend option was chosen else 2
** row		= front or back row. 2 front, 1 back
** DEFEND and BACK ROW reduce damage by half (stackable)
** target	= target that the action iis performed against
** gfxid	= enemy only, what sprite should be used.
*/

typedef struct			s_chara
{
	char				*name;
	int					maxhp;
	int					hp;
	int					row;
	int					action;
	int					defend;
	int					target;
	int					gfxid;
}						t_chara;

typedef struct			s_window
{
	t_uint32			wid;
	t_uint32			hgt;
	t_uint32			*pixels;
	SDL_Surface			*surface;
	SDL_Texture			*texture;
	SDL_Window			*window;
	SDL_Renderer		*render;
}						t_window;

typedef struct			s_gpuhandle
{
	cl_platform_id		platform_id;
	cl_uint				ret_num_devices;
	cl_uint				ret_num_platforms;
}						t_gpuhandle;

typedef struct			s_gpu
{
	cl_device_id		device_id;
	cl_context			context;
	cl_command_queue	commands;
	cl_program			program;
	cl_kernel			kernel;
	cl_mem				screen;	//uint32
	cl_mem				render;	//t_render
	cl_mem				area;	//char
	cl_mem				gfx;	//uint32
	size_t				local[2];
	size_t				global[2];
}						t_gpu;

typedef struct	s_wolf
{
	t_window	*win;
	t_render	*render;
	t_gpu		*gpu;
	SDL_Event	event;
	SDL_Thread	*logic_thread;
	SDL_mutex	*mutex;
	int			tile;
	t_gfx		*gfx;
	int			gfxcount;
	t_gfx		img;
	t_chara		*chara;
	void		(*cycle)(struct s_wolf*, struct s_gpu*);
	char		*syssmg[2];
	int			cur;
	int			sel;
	int			plr;
	int			plrck;
	int			flr;
#ifndef		RENDER_VECTOR
	char		***area;
#else
	t_map		*area;
#endif
	t_fpint		rng;
	int			aggro;
	t_fpint		movsp;
	t_fpint		rotsp;
	int			isclick;
	char		keys[SDL_NUM_SCANCODES];
}				t_wolf;

t_gfx			init_image(t_wolf *wolf, int x, int y);
t_gfx			gfx_get(t_wolf *wolf, char *file, int x, int y);
t_chara			*generate_party(t_wolf *wlf);
t_chara			generate_foe(t_wolf *wlf);

int				get_x(int pc);
int				get_y(int pc);
int				interact(t_wolf *wlf);
int				key_hold(int key, t_wolf *wlf);
int				key_press(int key, t_wolf *wolf);
int				key_release(int key, t_wolf *wolf);
int				move(t_wolf *wlf);
int				move_fb(t_wolf *wlf);
int				move_lr(t_wolf *wlf);
int				x_press(t_wolf *wolf);

char			*get_syssmgone(t_wolf *wlf, int pc);
char			*get_syssmgtwo(t_wolf *wlf, int pc);

void			anim_shift(t_wolf *wlf, int frame);
void			combat_key(int key, t_wolf *wlf);
void			comp_foe(t_wolf *wlf, char *bpath, int i);
void			comp_map(t_wolf *wolf, char *av);
void			cur_two(t_wolf *wlf, int tar);
void			cur_zero(t_wolf *wlf, int tar);
void			destroy_gfx(t_wolf *wlf, int i);
void			draw_gfx(t_wolf *wlf, t_gfx gfx, int x, int y);
void			encounter(t_wolf *wlf);
void			error_out(char *msg, t_wolf *wolf);
void			exit_combat(t_wolf *wlf);
void			free_layer(t_wolf *wlf, int f, int y, int mxy);
void			free_map(t_wolf *wlf, int f, int y);
void			free_memory(char **arr);
void			gen_att_ai(t_wolf *wlf);
void			health_check(t_wolf *wlf, int pc, int thp);
void			lab_move(t_wolf *wlf, int obj);
void			place_pc(t_wolf *wlf, int pc);
void			render(t_wolf *wlf);
void			render_floor(t_wolf *wlf, int y);
void			wall_stripe(t_wolf *wlf);

t_gpu			*init_gpu(t_window *win);
void			*destroy_gpu(t_gpu *gpu);
void			copy_map_to_gpu(t_wolf *wlf);
void			get_gpu_info(t_gpu *gpu, t_window *win);
void			prep_gpu(t_wolf *wlf, t_gpu *gpu);
cl_mem			comp_gfx(const char *file, t_gpu *gpu);
void			create_logic(t_wolf *wlf);
size_t			comp_vec_map(t_wolf *wlf, char *av);

#endif
