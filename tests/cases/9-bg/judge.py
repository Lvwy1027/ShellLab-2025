# trace09_judge.py
import json
import re
import sys


def judge():
    # 读取测试数据
    input_data = json.load(sys.stdin)
    stdout = input_data["stdout"]
    max_score = input_data["max_score"]

    # 定义要检查的关键状态转换
    key_patterns = [
        # 1. 后台作业启动
        r"\[1\] \(\d+\) \./myspin 4 &",
        # 2. 前台作业被信号停止
        r"Job \[2\] \(\d+\) stopped by signal (20|SIGTSTP)",
        # 3. jobs命令正确显示作业状态 (一个运行，一个停止)
        r"\[1\] \(\d+\) Running \./myspin 4 &[\s\S]*\[2\] \(\d+\) Stopped \./myspin 5",
        # 4. bg命令输出
        r"\[2\] \(\d+\) \./myspin 5",
        # 5. 两个作业都在运行状态
        r"\[1\] \(\d+\) Running \./myspin 4 &[\s\S]*\[2\] \(\d+\) Running \./myspin 5",
    ]

    # 检查每个模式
    missing_patterns = []
    for i, pattern in enumerate(key_patterns):
        if not re.search(pattern, stdout, re.MULTILINE):
            missing_patterns.append(f"Pattern {i + 1}")

    # 计算得分和状态
    if not missing_patterns:
        success = True
        message = "All state transitions verified successfully"
        score = max_score
    else:
        success = False
        message = f"Failed to verify: {', '.join(missing_patterns)}"
        # 根据通过的模式数量计算部分分数
        score = (
            max_score * (len(key_patterns) - len(missing_patterns)) / len(key_patterns)
        )

    # 返回结果
    result = {"success": success, "message": message, "score": score}

    print(json.dumps(result))


if __name__ == "__main__":
    judge()
