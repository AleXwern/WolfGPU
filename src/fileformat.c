/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileformat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/22 16:13:55 by anystrom          #+#    #+#             */
/*   Updated: 2021/04/30 15:46:07 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

void	validate_map(t_wolf *wlf, int i, int a)
{
	while (++a < wlf->mxflr)
	{
		i = -1;
		while (++i < wlf->width)
		{
			if (wlf->area[a][0][i] != 2)
				error_out(FIL_ERROR, wlf);
			if (wlf->area[a][wlf->height - 1][i] != 2)
				error_out(FIL_ERROR, wlf);
		}
		i = -1;
		while (++i < wlf->height)
		{
			if (wlf->area[a][i][0] != 2)
				error_out(FIL_ERROR, wlf);
			if (wlf->area[a][i][wlf->width - 1] != 2)
				error_out(FIL_ERROR, wlf);
		}
	}
}

int		templen(char **temp)
{
	int		i;

	i = 0;
	while (temp[i])
		i++;
	return (i);
}

int		get_next_matrix(t_wolf *wlf, char **temp, int x, int y)
{
	int		wid;

	wid = templen(temp);
	if (wlf->width == -1)
		wlf->width = wid;
	if (wid < 4 || wid >= 35 || wlf->width != wid)
		return (0);
	if (!(wlf->area[wlf->flr][y] = (char*)ft_memalloc(sizeof(char) * wid)))
		error_out(MEM_ERROR, wlf);
	while (temp[x])
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
		if (y >= 35)
			error_out(FIL_ERROR, wlf);
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
	if (wlf->height == -1)
		wlf->height = y;
	if (y != wlf->height || wlf->height < 4)
		error_out(FIL_ERROR, wlf);
}

void	comp_map(t_wolf *wlf, char *av)
{
	char	*flrfl;
	int		fd;

	wlf->height = -1;
	wlf->width = -1;
	if (!(wlf->area = (char***)ft_memalloc(sizeof(char**) * wlf->mxflr)))
		error_out(MEM_ERROR, wlf);
	while (wlf->flr < wlf->mxflr)
	{
		if (wlf->flr >= wlf->mxflr)
			return ;
		if (!(wlf->area[wlf->flr] = (char**)ft_memalloc(sizeof(char*) * 35)))
			error_out(MEM_ERROR, wlf);
		wlf->flr += 49;
		flrfl = ft_quadjoin(av, "/", (char*)&(wlf->flr), "");
		fd = open(flrfl, O_RDONLY);
		free(flrfl);
		if (fd == -1)
			error_out(FLR_ERROR, wlf);
		wlf->flr -= 49;
		fileformat(fd, wlf, 0);
		wlf->flr++;
	}
	validate_map(wlf, -1, -1);
}
