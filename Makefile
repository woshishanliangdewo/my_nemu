STUID = 12020242542
STUNAME = 刘胤宗

# DO NOT modify the following code!!!

GITFLAGS = -q --author='tracer-ics2022 <tracer@njuics.org>' --no-verify --allow-empty

# prototype: git_commit(msg)
define git_commit
	-@git add $(NEMU_HOME)/.. -A --ignore-errors
	-@while (test -e .git/index.lock); do sleep 0.1; done
	-@(echo "> $(1)" && echo $(STUID) $(STUNAME) && uname -a && uptime) | git commit -F - $(GITFLAGS)
	-@sync
endef

_default:
# 作用是使得echo ... 这个命令执行前不会进行展示，也就是不会显示echo了，且不会显示两次
	@echo "Please run 'make' under subprojects."

submit:
	git gc
	STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s http://why.ink:8080/static/submit.sh)"
# 一般一个makefile中的一个目标是通过make后跟目标名实现的，什么是目标？ 就是每一个部分的那个整体的名字么。
# 但是make的最终目标一般是makefile的第一个目标，第一个目标通常用来集成。
# 所有以- 或者 = 开头的解析为命令行参数或变量。
# 但是有目标，一定就会有伪目标，比如说.PHONY,他的作用是指定一个目标是伪目标。也就是说不管有没有他都是伪目标。
# 之所以叫伪目标是因为目标不会生成文件，因此无法生成依赖关系
# 为万待续。
.PHONY: default submit
