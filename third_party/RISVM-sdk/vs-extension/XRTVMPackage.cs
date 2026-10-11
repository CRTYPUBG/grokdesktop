using System;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System.Threading;
using Task = System.Threading.Tasks.Task;

namespace XRTVMExtension
{
    /// <summary>
    /// XRTVM Visual Studio Extension package.
    /// Registers the XrtvmPostBuildTask MSBuild task and integrates
    /// the xrtvm-cli compile step as a post-build action.
    /// </summary>
    [PackageRegistration(UseManagedResourcesOnly = true, AllowsBackgroundLoading = true)]
    [Guid(XRTVMPackage.PackageGuidString)]
    [ProvideAutoLoad(VSConstants.UICONTEXT.SolutionExistsAndFullyLoaded_string,
        PackageAutoLoadFlags.BackgroundLoad)]
    [ProvideMenuResource("Menus.ctmenu", 1)]
    public sealed class XRTVMPackage : AsyncPackage
    {
        public const string PackageGuidString = "a1b2c3d4-e5f6-7890-abcd-ef1234567890";

        protected override async Task InitializeAsync(
            CancellationToken cancellationToken,
            IProgress<ServiceProgressData> progress)
        {
            await base.InitializeAsync(cancellationToken, progress);

            // Switch to UI thread for VS shell interactions
            await JoinableTaskFactory.SwitchToMainThreadAsync(cancellationToken);

            // Log that the extension loaded successfully
            var outputWindow = await GetServiceAsync(typeof(SVsOutputWindow)) as IVsOutputWindow;
            if (outputWindow != null)
            {
                var paneGuid = VSConstants.OutputWindowPaneGuid.GeneralPane_guid;
                outputWindow.GetPane(ref paneGuid, out var pane);
                pane?.OutputStringThreadSafe("[XRTVM Extension] Loaded successfully.\n");
            }
        }
    }
}
