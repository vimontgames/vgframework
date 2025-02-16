using Sharpmake;

namespace vg
{
    [Sharpmake.Generate]
    public class Game : Project
    {
        public Game() : base("game", Type.Game)
        {

        }

        public override void ConfigureAll(Configuration conf, Target target)
        {
            base.ConfigureAll(conf, target);
            conf.AddPrivateDependency<Core>(target);
        }
    }
}