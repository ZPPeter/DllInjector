# DllInjector

DllInjector 是一个用于将 DLL 注入到目标进程的工具。通过注入 DLL，您可以在目标进程中执行自定义代码的功能。

## 使用方法

1. 下载最新版本的 DllInjector 可执行文件：[下载链接](https://github.com/furacas/DllInjector/releases/latest)。

2. 准备要注入的 DLL 文件，并将其与 DllInjector 可执行文件放在同一目录中。

3. 使用命令行或资源管理器打开包含 DllInjector 可执行文件和 DLL 文件的目录。

4. 执行以下命令：

    ```bash
    DllInjector.exe <dll_path> <process_name>
    ```

    - `<dll_path>`：DLL 文件的路径，可以是相对路径或绝对路径。
    - `<process_name>`：要注入的目标进程的名称。

    例如，要将名为 `mydll.dll` 的 DLL 注入到名为 `target.exe` 的进程中，运行以下命令：

    ```bash
    DllInjector.exe mydll.dll target.exe
    ```


5. 程序将输出注入结果，如果注入成功，您将看到 "DLL injected successfully." 的提示信息。

## 注意事项

- 在使用 DllInjector 进行注入时，请确保您具有足够的权限来打开目标进程并执行注入操作。
- 在使用 DllInjector 之前，请遵守法律法规和道德规范，并确保您的行为合法和道德。
