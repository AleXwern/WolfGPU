/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floor.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/03/24 01:20:40 by AleXwern          #+#    #+#             */
/*   Updated: 2021/05/01 18:37:48 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

void	draw_floor(t_wolf *wlf, int y)
{
	int		x;

	x = 0;
	while (x < WINX)
	{
		if (wlf->texbool)
		{
			wlf->cellx = (int)wlf->floorx;
			wlf->celly = (int)wlf->floory;
			wlf->tx = (int)(128 * (wlf->floorx - wlf->cellx)) & (128 - 1);
			wlf->ty = (int)(128 * (wlf->floory - wlf->celly)) & (128 - 1);
			wlf->floorx += wlf->flstepx;
			wlf->floory += wlf->flstepy;
			wlf->img.data[WINX * y + x] = wlf->gfx[1].data[128 *
					wlf->ty + wlf->tx];
		}
		else
			wlf->img.data[WINX * y + x] = 0x0f9926;
		x++;
	}
}

void	render_floor(t_wolf *wlf, int y)
{
	y = WINY / 2;
	wlf->raydx0 = wlf->dirx - wlf->planex;
	wlf->raydy0 = wlf->diry - wlf->planey;
	wlf->raydx1 = wlf->dirx + wlf->planex;
	wlf->raydy1 = wlf->diry + wlf->planey;
	while (y < WINY)
	{
		wlf->pos = y - WINY / 2;
		wlf->posz = WINY * 0.5;
		wlf->rowdist = wlf->posz / wlf->pos;
		wlf->flstepx = wlf->rowdist * (wlf->raydx1 - wlf->raydx0) / WINX;
		wlf->flstepy = wlf->rowdist * (wlf->raydy1 - wlf->raydy0) / WINX;
		wlf->floorx = wlf->posx + wlf->rowdist * wlf->raydx0;
		wlf->floory = wlf->posy + wlf->rowdist * wlf->raydy0;
		draw_floor(wlf, y);
		y++;
	}
}

void	move_skybox(t_wolf *wlf)
{
	double	adjy;

	wlf->sbox = 0;
	adjy = (wlf->diry + 1.0) / 2;
	if (wlf->dirx > 0)
	{
		wlf->sbox = WINX * adjy;
	}
	else
	{
		wlf->sbox = WINX - WINX * adjy;
	}
}

/*DOOM
void	draw_floor(t_doom *dm)
{
	if (dm->texbool)
	{
		dm->cellx = (int)dm->floor.x;
		dm->celly = (int)dm->floor.y;
		dm->tx = (int)(128 * (dm->floor.x - dm->cellx)) & (128 - 1);
		dm->ty = (int)(128 * (dm->floor.y - dm->celly)) & (128 - 1);
		dm->col = color_shift(dm->gfx[dm->texnum].data[128 * dm->ty +
			dm->tx], dm->walldist + fabs((double)(dm->x - dm->winw / 2) /
			dm->winw), dm, 0);
		dm->lgt = light_map(dm->map, dm->side, dm->area);
		dm->col = rl_color(dm->lgt, dm->col);
	}
	else if (dm->area[(int)dm->map.z][(int)dm->map.y][(int)dm->map.x].b > 2)
		dm->col = 0xff22a800;
	if (dm->side > 2)
		dm->col = (dm->col >> 1) & DARKEN;
	dm->img.data[dm->winw * dm->y + dm->x] = dm->col;
}

void	render_floor(t_doom *dm)
{
	dm->texnum = 1;
	if (dm->area[(int)dm->map.z][(int)dm->map.y][(int)dm->map.x].b == 6)
		dm->texnum = 6;
	dm->rayd0.x = dm->dir.x - dm->plane.x;
	dm->rayd0.y = dm->dir.y - dm->plane.y;
	dm->rayd1.x = dm->dir.x + dm->plane.x;
	dm->rayd1.y = dm->dir.y + dm->plane.y;
	dm->flstep.x = dm->walldist * (dm->rayd1.x - dm->rayd0.x) / dm->winw;
	dm->flstep.y = dm->walldist * (dm->rayd1.y - dm->rayd0.y) / dm->winw;
	dm->floor.x = (dm->pos.x + dm->walldist * dm->rayd0.x)
		+ (dm->flstep.x * dm->x);
	dm->floor.y = (dm->pos.y + dm->walldist * dm->rayd0.y)
		+ (dm->flstep.y * dm->x);
	draw_floor(dm);
	if (dm->wincol)
		ext_ray(dm);
}*/
