using Flax.Build;

public class ArizonaFrameworkEditorTarget : GameProjectEditorTarget
{
    /// <inheritdoc />
    public override void Init()
    {
        base.Init();

        Modules.Add("ArizonaFramework");
    }
}
