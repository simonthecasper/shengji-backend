FROM python:3.12

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y iputils-ping
RUN apt-get install -y ufw
RUN ufw allow from any to any port 2500 proto tcp
RUN ufw allow from any to any port 2000 proto tcp


WORKDIR /shengji-backend_middleman
COPY . .

RUN pip install aiohttp
RUN python3 -m pip install python-socketio

EXPOSE 2500

CMD ["python3", "-u","sjbackend_middleman.py"]