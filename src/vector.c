/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anystrom <anystrom@hive.fi>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/03 17:37:50 by anystrom          #+#    #+#             */
/*   Updated: 2021/05/03 17:37:50 by anystrom         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/wolf.h"
#include "../includes/value.h"

#ifdef RENDER_VECTOR

t_vector		*get_vectors(int fd, size_t amount)
{
	t_vector	*vector;
	char		*str;
	//char		**vec;

	vector = (t_vector*)ft_memalloc(sizeof(t_vector) * amount);
	for (size_t i = 0; i < amount; i++)
	{
		get_next_line(fd, &str);
		//if (*((Uint32*)str) == VEC_DELIM)
		//	break;
		vector[i].y = ft_atof(str);
		vector[i].x = ft_atof(ft_strchr(str, ',') + 1);
		free(str);
		printf("Vector %u: %f,%f\n", i, vector[i].x, vector[i].y);
	}
	return (vector);
}

size_t			comp_vec_map(t_wolf *wlf, char *av)
{
	char		*gnl;
	char		**header;
	int			fd;
	t_vector	*vector;
	size_t		sizes[2];

	fd = open("./map3d/vec0", O_RDONLY);
	if (get_next_line(fd, &gnl) != 1)
		error_out(FIL_ERROR, wlf);
	header = ft_strsplit(gnl, ',');
	free(gnl);
	sizes[0] = ft_atoi(header[0]);
	sizes[1] = ft_atoi(header[1]);
	printf("%u %d\n", sizes[0], sizes[1]);
	vector = get_vectors(fd, sizes[0]);
	wlf->area = (t_map*)ft_memalloc(sizeof(t_map) * sizes[1]);
	ft_splitfree(header);
	for (size_t i = 0; i < sizes[1]; i++)
	{
		get_next_line(fd, &gnl);
		wlf->area[i].p1 = vector[ft_atoi(gnl)];
		wlf->area[i].p2 = vector[ft_atoi(ft_strchr(gnl, ',') + 1)];
		wlf->area[i].p2.x -= wlf->area[i].p1.x;
		wlf->area[i].p2.y -= wlf->area[i].p1.y;
		printf("vec %u: %f,%f - %f,%f\n", i, wlf->area[i].p1.x, wlf->area[i].p1.y, wlf->area[i].p2.x, wlf->area[i].p2.y);
		free(gnl);
	}
	free(vector);
	return (sizes[1]);
}

#endif
