# ShellLab：构建 Unix Shell 🐚

## 实验说明文档

### 1. 实验目标 🎯

通过编写一个支持作业控制的Unix Shell程序，深入理解进程控制和信号处理的核心概念。

### 2. 实验概述 📝

Shell是一个交互式命令行解释器，负责接收用户指令并执行相应程序。本实验要求实现一个简化版的Shell，包含基本的命令解析、进程创建、信号处理、作业控制和I/O重定向功能。实验提供了骨架代码和完整的命令解析器，你需要实现核心执行逻辑和控制机制。

关于Shell原理和实现方法的详细指导，请参阅[实验指导文档](docs/guide.md)。

### 3. 实验要求 📋

#### 3.1 基本功能（必做）

- 命令提示符为`"tsh> "`
- 执行命令行程序（前台或后台运行）
- 实现作业控制（bg、fg等）
- 处理键盘信号（Ctrl+C、Ctrl+Z）
- 实现基本的I/O重定向（`<`、`>`）
- 回收终止的子进程

#### 3.2 内建命令

- `quit`/`exit`：终止shell
- `jobs`：列出所有后台作业
- `bg <job>`：将一个停止状态的后台作业转为运行状态
- `fg <job>`：将一个后台作业转为前台运行
- `cd <dir>`：切换当前工作目录

#### 3.3 附加任务（选做）

- 基本管道支持（`|`）
- 复杂管道链支持（如`cmd1 | cmd2 | cmd3`）
- 环境变量展开功能（`$VAR`或`${VAR}`）
- 命令替换功能（`$(command)`）
- 终端控制机制（支持vim、gdb等交互式程序）
- 脚本执行功能
- 支持PATH环境变量

### 4. 文件结构与接口 🗂️

项目采用模块化设计，文件结构如下：

```
.
├── include
│   └── shell.h     # 头文件，包含函数声明与数据结构
├── Makefile        # 用于编译项目
└── src
    ├── builtins.c  # 内置命令的实现
    ├── jobs.c      # 作业控制相关函数
    ├── main.c      # 主函数
    ├── parser.c    # 命令解析相关函数
    ├── shell.c     # Shell 核心逻辑
    ├── signals.c   # 信号处理函数
    └── utils.c     # 工具函数
```

命令解析器提供了以下结构：

```c
typedef struct command {
  char *argv[MAXARGS];  /* 命令和参数 */
  int argc;             /* 参数数量 */
  char *infile;         /* 输入重定向文件 */
  char *outfile;        /* 输出重定向文件 */
  int append;           /* 输出重定向的追加模式标志 */
  struct command *next; /* 管道：指向下一个命令 */
} command_t;

int parse_command_line(const char *cmdline, command_t **cmd, int *bg);
```

需要实现的主要函数：

- `builtins.c`: `builtin_cmd`, `do_bgfg`
- `shell.c`: `eval`, `eval_script`（选做）
- `signals.c`: `sigchld_handler`, `sigint_handler`, `sigtstp_handler`
- `parser.c`: `env_expand`, `command_substitute`（选做）

### 5. 测试方法 🧪

实验提供了测试脚本，可以用于验证你的 Shell 实现是否正确：

```bash
python grader.py
```

或者使用如下的方式运行单个测试点：

```bash
python grader.py <test_id>
```

### 6. 评分标准 💯

- 基础功能实现（60%）
  - 命令执行与进程管理
  - 信号处理
  - 作业控制
  - I/O重定向
  - 代码风格与注释

- 选做任务（20%）
  - 任选两项选做任务完成
  - 或使用 Rust 实现

- 实验报告与代码质量（20%）
  - 实现思路清晰
  - 关键功能分析
  - 测试结果
  - 代码结构合理
  - 注释完善
  - 错误处理充分

### 7. 提交方式 📤

使用 GitHub Classroom 进行提交。请你确保所有代码已提交到你的对应仓库，GitHub Actions 会自动运行测试，其输出作为我们的评分依据。

提交截止日期：2025年3月21日 23:59

提交内容：
1. 所有源代码文件（通过 GitHub 仓库提交）
2. 实验报告（PDF格式，上传至仓库根目录）

请确保在截止日期前完成最终提交。GitHub 会记录你的所有提交历史，我们将以截止日期前的最后一次提交作为最终版本进行评分。
