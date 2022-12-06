using Flax.Build;
using Flax.Build.NativeCpp;

public class ArizonaFramework : GameModule
{
    /// <inheritdoc />
    public override void Setup(BuildOptions options)
    {
        base.Setup(options);

        BuildNativeCode = true;
        Tags["Network"] = string.Empty;
        options.PublicDependencies.Add("Networking");
        options.PrivateDependencies.Add("ImGui");
        options.ScriptingAPI.IgnoreMissingDocumentationWarnings = false;
    }
}
