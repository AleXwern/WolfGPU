/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileformat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/22 16:13:55 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/04 16:12:15 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

#ifndef RENDER_VECTOR

void	validate_map(t_wolf *wlf, int i, int a)
{
	while (++a < MAXDEPTH)
	{
		i = -1;
		while (++i < MAXWIDTH)
		{
			wlf->area[a][0][i] = 2;
			wlf->area[a][MAXHEIGHT - 1][i] = 2;
		}
		i = -1;
		while (++i < MAXHEIGHT)
		{
			wlf->area[a][i][0] = 2;
			wlf->area[a][i][MAXWIDTH - 1] = 2;
		}
	}
	for (size_t i = 0; i < MAXHEIGHT; i++)
	{
		ft_memset(wlf->area[0][i], 2, MAXWIDTH);
		ft_memset(wlf->area[MAXDEPTH - 1][i], 2, MAXWIDTH);
	}
	
}

char	***create_map(void)
{
	char	***area;
	
	if (!(area = (char***)ft_memalloc(sizeof(char**) * MAXDEPTH)))
		return (NULL);
	for (int z = 0; z < MAXDEPTH; z++)
	{
		if (!(area[z] = (char**)ft_memalloc(sizeof(char*) * MAXHEIGHT)))
			return (NULL);
		for (int y = 0; y < MAXHEIGHT; y++)
		{
			if (!(area[z][y] = (char*)malloc(sizeof(char) * MAXWIDTH)))
				return (NULL);
			ft_memset(area[z][y], 2, MAXWIDTH);
		}
	}
	return (area);
}

int		get_next_matrix(t_wolf *wlf, char **temp, int x, int y)
{
	while (temp[x] && x < MAXWIDTH)
	{
		wlf->area[wlf->flr][y][x] = ft_atoi(temp[x]);
		if (wlf->area[wlf->flr][y][x] < 1 || wlf->area[wlf->flr][y][x] > 6)
			wlf->area[wlf->flr][y][x] = 2;
		x++;
	}
	return (1);
}

void	fileformat(int fd, t_wolf *wlf, int y)
{
	char	**temp;
	char	*gnl;

	while (get_next_line(fd, &gnl) == 1)
	{
		if (y >= MAXHEIGHT)
		{
			free(gnl);
			return;
		}
		temp = ft_strsplit(gnl, ' ');
		free(gnl);
		if (get_next_matrix(wlf, temp, 0, y) == 0)
		{
			ft_splitfree(temp);
			error_out(FIL_ERROR, wlf);
		}
		y++;
		ft_splitfree(temp);
	}
}

void	comp_map(t_wolf *wlf, char *av)
{
	char	*flrfl;
	int		fd;

	if (!(wlf->area = create_map()))
		error_out(MEM_ERROR, wlf);
	while (wlf->flr < MAXDEPTH)
	{
		wlf->flr += 49;
		flrfl = ft_quadjoin(av, "/", (char*)&(wlf->flr), "");
		fd = open(flrfl, O_RDONLY);
		free(flrfl);
		wlf->flr -= 49;
		fileformat(fd, wlf, 0);
		close(fd);
		wlf->flr++;
	}
	validate_map(wlf, -1, -1);
}
#endif
