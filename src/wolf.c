/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wolf.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/24 15:01:06 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/01 20:00:51 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

//printf("%s - %d\n", __FILE__, __LINE__);

void	wolf_default(t_wolf *wlf)
{
	wlf->flr = 0;
	wlf->render = (t_render*)ft_memalloc(sizeof(t_render));
	wlf->render->pos.x = 2.5;
	wlf->render->pos.y = 2.5;
	wlf->render->dir.x = 1.0;
	wlf->render->dir.y = 0.0;
	wlf->render->plane.x = 0.0;
	wlf->render->plane.y = 0.66;
	wlf->render->maxx = wlf->win->wid;
	wlf->render->maxy = wlf->win->hgt;
	wlf->render->texbool = 1;
	wlf->rotsp = 0.004;
	wlf->movsp = 0.004;
	wlf->rng = 0.0;
	wlf->sbox = WINX / 2;
	wlf->mxflr--;
	wlf->cur = 0;
	wlf->sel = -1;
	wlf->plr = 0;
	wlf->plrck = 0;
	wlf->cycle = &prep_gpu;
	//wlf->chara = generate_party(wlf);
	wlf->syssmg[0] = ft_strdup("You encountered a strong beast!");
	wlf->syssmg[1] = ft_strdup("What will you do?");
}

void	error_out(char *msg, t_wolf *wlf)
{
	destroy_gpu(wlf->gpu);
	SDL_Quit();
	ft_putendl(msg);
	exit(0);
}

void	init_window(t_wolf *wlf)
{
	t_window	*win;
	
	if (!(win = (t_window*)ft_memalloc(sizeof(t_window))))
		error_out(MEM_ERROR, wlf);
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER))
		error_out("SDL error", wlf);
	if (!(win->window = SDL_CreateWindow("wolfGPU", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, 1080, 720, SDL_WINDOW_FULLSCREEN_DESKTOP)))
		error_out("Window error.", wlf);
	if (!(win->render = SDL_CreateRenderer(win->window, -1, SDL_RENDERER_SOFTWARE)))
		if (!(win->render = SDL_GetRenderer(win->window)))
			error_out("Renderer error.", wlf);
	if (!(win->surface = SDL_GetWindowSurface(win->window)))
		error_out("Surface error", wlf);
	win->texture = SDL_CreateTextureFromSurface(win->render, win->surface);
	win->pixels = (t_uint32*)win->surface->pixels;
	wlf->win = win;
	win->wid = win->surface->w;
	win->hgt = win->surface->h;
}

void	setup(t_wolf *wlf)
{
	Uint32	start;
	Uint32	now;
	Uint32	fps = 0;
	
	init_window(wlf);
	wolf_default(wlf);
	wlf->gpu = init_gpu(wlf->win);	//GPU stuff after this point
	if (!wlf->gpu)
		error_out("GPU init not succesful", wlf);
	copy_map_to_gpu(wlf);
	wlf->gpu->gfx = comp_gfx("./gfx/graphics.bmp", wlf->gpu);
	wlf->area[0][(int)wlf->render->pos.y][(int)wlf->render->pos.x] = 1;
	start, now = SDL_GetTicks();
	while (1)
	{
		//prep_gpu(wlf, wlf->gpu);
		wlf->cycle(wlf, wlf->gpu);
		fps++;
		now = SDL_GetTicks();
		if (now - start > 1000)
		{
			printf("FPS: %u\n", fps);
			fps = 0;
			start = SDL_GetTicks();
		}
	}
}

int			main(int ac, char **av)
{
	t_wolf		wlf;

	ft_bzero(&wlf, sizeof(t_wolf));
	/*if (ac != 4)
		error_out(USAGE, &wlf);
	wlf.tile = ft_atoi(av[1]);
	if (wlf.tile < 1 || wlf.tile > 6)
		error_out(USAGE, &wlf);
	wlf.mxflr = ft_atoi(av[2]);
	if (wlf.mxflr < 1 || wlf.mxflr > 9)
		error_out(USAGE, &wlf);*/
	wlf.mxflr = 1;
	comp_map(&wlf, "./map3d/");//av[3]);
	//comp_gfx(&wlf, 0);
	setup(&wlf);
	error_out("fine", &wlf);
	return (0);
}
