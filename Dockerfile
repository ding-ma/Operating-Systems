# CLion remote docker environment (How to build docker container, run and stop it)
#
# Build and run:
#   docker build -t clion/remote-cpp-env:0.5 -f Dockerfile .
#   docker run -d --cap-add sys_ptrace -p127.0.0.1:3333:22 --name clion_remote_env clion/remote-cpp-env:0.5
#   ssh-keygen -f "$HOME/.ssh/known_hosts" -R "[localhost]:3333"
#
# stop:
#   docker stop clion_remote_env
#
# ssh credentials (test user):
#   user@password

FROM ubuntu:18.04

RUN apt-get update \
  && apt-get install -y ssh \
      git \
      git ansible \
  && apt-get clean

RUN git clone https://github.com/trungams/bionic-image && cd bionic-image && ansible-playbook -i ./hosts main.yml

# if git does not work, uncomment following 2 line
#COPY ansible /user/local
#RUN cd /user/local && ansible-playbook -i ./hosts main.yml

RUN ( \
    echo 'LogLevel DEBUG2'; \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Subsystem sftp /usr/lib/openssh/sftp-server'; \
  ) > /etc/ssh/sshd_config_test_clion \
  && mkdir /run/sshd

RUN useradd -m user \
  && yes password | passwd user

CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_test_clion"]