# Branch and release workflow
main is the accepted release line; development integrates upcoming work. Start feature/<topic> from development and merge validated slices back into development.
For a release, verify feature-map acceptance and retained build/runtime/visual evidence, merge the accepted development revision to main, then create an annotated vX.Y.Z tag on that main commit. Tag messages identify acceptance and artifact evidence. Do not tag planning scaffolds as releases.
Use fix/<topic> for urgent release fixes and merge the fix back into development. Branch names are independent of the themes/master asset directory.
Keep commits bounded and task records current. Initial local bootstrap commits establish the repository; they are not product releases. No automatic remote push, deployment or publication follows a merge/tag.
