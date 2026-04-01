#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int	main()
{
	char	password[1024] = "";
	int		fd = open("token", O_RDONLY);
	int		i = 0;
	char	decrypted[1024] = "";
	if (fd < 0)
		return (1);
	if (read(fd, password, 1024) < 0)
		return (2);
	while (password[i] != '\0' && password [i] != '\n')
	{
		decrypted[i] = password[i] - i;
		i++;
	}
	decrypted[i] = 0;
	printf("your token is : %s\n", decrypted);
	close(fd);
	return (0);
}
