import json
import re
import sys


def judge():
    # 读取测试数据
    input_data = json.load(sys.stdin)
    stdout = input_data["stdout"]
    max_score = input_data["max_score"]

    # 定义要检查的关键模式
    # 1. 输出重定向：应该能看到 "Hello, World!" 被正确输出
    # 2. 输入重定向：应该能看到 "Test input redirection" 被正确读取
    key_patterns = [r"Hello, World!", r"Test input redirection"]

    # 查找 shell 提示符模式，以便正确识别命令输出
    prompt_pattern = r"[\w\d\-_]+[@:][\w\d\-_/]+[#$%>]"
    prompt_matches = re.findall(prompt_pattern, stdout)
    prompt = None
    if prompt_matches:
        # 使用最常见的提示符
        from collections import Counter

        prompt = Counter(prompt_matches).most_common(1)[0][0]

    # 检查每个模式
    missing_patterns = []
    for i, pattern in enumerate(key_patterns):
        if not re.search(pattern, stdout, re.MULTILINE):
            missing_patterns.append(f"Pattern {i + 1}: {pattern}")

    # 检查是否有错误信息
    error_patterns = [
        r"[Ee]rror",
        r"[Ff]ailed",
        r"[Cc]annot",
        r"[Nn]o such file",
        r"[Pp]ermission denied",
    ]

    errors_found = []
    for pattern in error_patterns:
        matches = re.findall(pattern, stdout, re.MULTILINE | re.IGNORECASE)
        if matches:
            # 过滤掉可能在命令中包含的错误词（如 echo "Error message"）
            # 只考虑在提示符后出现的错误
            if prompt:
                real_errors = []
                for match in matches:
                    # 查找匹配项前面最近的提示符
                    match_pos = stdout.find(match)
                    last_prompt_pos = stdout.rfind(prompt, 0, match_pos)
                    if last_prompt_pos != -1:
                        command_line = stdout[last_prompt_pos:match_pos].strip()
                        # 如果错误不是命令的一部分，则认为是真正的错误
                        if not (("echo" in command_line) and (match in command_line)):
                            real_errors.append(match)
                if real_errors:
                    errors_found.append(f"Error pattern: {pattern}")
            else:
                errors_found.append(f"Error pattern: {pattern}")

    # 计算得分和状态
    if not missing_patterns and not errors_found:
        success = True
        message = "All I/O redirection tests passed successfully"
        score = max_score
    else:
        success = False
        message_parts = []
        if missing_patterns:
            message_parts.append(
                f"Missing expected output: {', '.join(missing_patterns)}"
            )
        if errors_found:
            message_parts.append(f"Errors detected: {', '.join(errors_found)}")
        message = "; ".join(message_parts)

        # 根据通过的测试数量计算部分分数
        total_checks = len(key_patterns) + (1 if errors_found else 0)
        passed_checks = (
            len(key_patterns) - len(missing_patterns) + (0 if errors_found else 1)
        )
        score = max_score * (passed_checks / total_checks)

    # 返回结果
    result = {"success": success, "message": message, "score": score}

    print(json.dumps(result))


if __name__ == "__main__":
    judge()
