const vscode = require('vscode');
const cp = require('child_process');

/**
 * @param {vscode.ExtensionContext} context
 */
function activate(context) {
    // Register the command
    let disposable = vscode.commands.registerCommand('cppLinecount.showLineCounts', function () {
        // PowerShell script to count lines in .cpp/.hpp files in main/
        const script = 'Get-ChildItem .\\main\\*.cpp, .\\main\\*.hpp | Sort-Object Name | ForEach-Object { "$($_.Name): $((Get-Content $_.FullName | Measure-Object -Line).Lines)" }';
        cp.exec(`powershell -Command "${script}"`, { cwd: vscode.workspace.rootPath }, (err, stdout, stderr) => {
            if (err) {
                vscode.window.showErrorMessage('Error running line count script: ' + err.message);
                return;
            }
            if (stderr) {
                vscode.window.showWarningMessage('Script warning: ' + stderr);
            }
            // Show results in output channel
            const output = vscode.window.createOutputChannel('CPP Linecount');
            output.clear();
            output.appendLine('Line counts for .cpp/.hpp files in main/:');
            output.appendLine(stdout);
            output.show();
        });
    });

    context.subscriptions.push(disposable);

    // Add a status bar button
    const statusBarItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Left, 100);
    statusBarItem.text = '$(list-unordered) CPP Linecount';
    statusBarItem.command = 'cppLinecount.showLineCounts';
    statusBarItem.tooltip = 'Show line counts for .cpp/.hpp files in main/';
    statusBarItem.show();
    context.subscriptions.push(statusBarItem);
}

function deactivate() {}

module.exports = {
    activate,
    deactivate
};
