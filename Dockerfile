FROM debian:sid

RUN apt-get update
RUN apt-get install -y\
	curl\
	wget\
	nodejs\
	liburing-dev\
	build-essential\
	cmake\
	ccls\
	git\
	neovim\
	fzf\
	python3\
	python3-venv
RUN git clone --depth 1 https://github.com/wbthomason/packer.nvim\
 ~/.local/share/nvim/site/pack/packer/start/packer.nvim
COPY init.lua /root/.config/nvim/init.lua
RUN nvim --headless -c 'autocmd User PackerComplete quitall' -c 'PackerSync'
COPY startup.lua .
RUN cat startup.lua >> /root/.config/nvim/init.lua
CMD tail -f /dev/null
