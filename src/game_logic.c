/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_logic.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/01 22:22:57 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/01 22:22:57 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

void		move_player(t_wolf *wlf)
{
	t_fpint	olddirx;
	t_fpint	oldplanex;

	if (wlf->keys[SDL_SCANCODE_UP])
	{
		wlf->render->pos.y += wlf->render->dir.y * wlf->movsp;
		wlf->render->pos.x += wlf->render->dir.x * wlf->movsp;
	}
	if (wlf->keys[SDL_SCANCODE_DOWN])
	{
		wlf->render->pos.y -= wlf->render->dir.y * wlf->movsp;
		wlf->render->pos.x -= wlf->render->dir.x * wlf->movsp;
	}
	if (wlf->keys[SDL_SCANCODE_LEFT])
	{
		olddirx = wlf->render->dir.x;
		wlf->render->dir.x = wlf->render->dir.x * cos(-wlf->rotsp) - wlf->render->dir.y * sin(-wlf->rotsp);
		wlf->render->dir.y = olddirx * sin(-wlf->rotsp) + wlf->render->dir.y * cos(-wlf->rotsp);
		oldplanex = wlf->render->plane.x;
		wlf->render->plane.x = wlf->render->plane.x * cos(-wlf->rotsp) - wlf->render->plane.y *
				sin(-wlf->rotsp);
		wlf->render->plane.y = oldplanex * sin(-wlf->rotsp) + wlf->render->plane.y *
				cos(-wlf->rotsp);
	}
	if (wlf->keys[SDL_SCANCODE_RIGHT])
	{
		olddirx = wlf->render->dir.x;
		wlf->render->dir.x = wlf->render->dir.x * cos(wlf->rotsp) - wlf->render->dir.y * sin(wlf->rotsp);
		wlf->render->dir.y = olddirx * sin(wlf->rotsp) + wlf->render->dir.y * cos(wlf->rotsp);
		oldplanex = wlf->render->plane.x;
		wlf->render->plane.x = wlf->render->plane.x * cos(wlf->rotsp) - wlf->render->plane.y *
				sin(wlf->rotsp);
		wlf->render->plane.y = oldplanex * sin(wlf->rotsp) + wlf->render->plane.y *
				cos(wlf->rotsp);
	}
	if (wlf->keys[SDL_SCANCODE_T])
	{
		wlf->render->texbool = wlf->render->texbool ? 0 : 1;
		wlf->keys[SDL_SCANCODE_T] = 0;
	}
	/*if (wlf->render->pos.y < 0)
		wlf->render->pos.y = 25;
	else if (wlf->render->pos.y > 25)
		wlf->render->pos.y = 0;
	if (wlf->render->pos.x < 0)
		wlf->render->pos.x = 25;
	else if (wlf->render->pos.x > 25)
		wlf->render->pos.x = 0;*/
}

int		game_logic(void *ptr)
{
	t_wolf	*wlf;
	Uint32	last;

	wlf = (t_wolf*)ptr;
	while (1)
	{
		last = SDL_GetTicks();
		SDL_LockMutex(wlf->mutex);
		move_player(wlf);
		SDL_UnlockMutex(wlf->mutex);
		last = SDL_GetTicks() - last;
		SDL_Delay(2 - last);
	}
	return (1);
}

void	create_logic(t_wolf *wlf)
{
	wlf->mutex = SDL_CreateMutex();
	if (!wlf->mutex)
		error_out("Couldn't create mutex!", wlf);
	wlf->logic_thread = SDL_CreateThread(game_logic, "Game Logic", wlf);
	if (!wlf->logic_thread)
		error_out("Couldn't create logic thread!", wlf);
}
