docker build -t ldpl .
docker run -it --rm -v ./:/app --name ldpl ldpl /bin/sh
