﻿// Copyright (c) Ubisoft. All Rights Reserved.
// Licensed under the Apache 2.0 License. See LICENSE.md in the project root for license information.

using System;
using System.IO;

namespace Sharpmake
{
    /// <summary>
    /// Interface that exposes facilities for generating text-based files.
    /// </summary>
    public interface IFileGenerator
    {
        /// <summary>
        /// Gets the <see cref="Sharpmake.Resolver"/> instance that the generator uses to resolve
        /// the symbols between square brackets `[...]` when <see cref="Write(string)"/> and
        /// <see cref="WriteLine(string)"/> are called.
        /// </summary>
        Resolver Resolver { get; }

        /// <summary>
        /// Writes text to the file being generated, resolving the symbols between square brackets
        /// `[...]` along the way.
        /// </summary>
        /// <param name="text">The text to write into the file.</param>
        void Write(string text);

        /// <summary>
        /// Writes text to the file being generated, resolving the symbols between square brackets
        /// `[...]` along and way. Uses a default value when the resolve process fails.
        /// </summary>
        /// <param name="text">The text to write into the file.</param>
        /// <param name="fallbackValue">A <see cref="string"/> to use when the resolve fails</param>
        void Write(string text, string fallbackValue);

        /// <summary>
        /// Writes a line of text to the file being generated, resolving the symbols between square
        /// brackets `[...]` along the way.
        /// </summary>
        /// <param name="text">The line of text to write into the file.</param>
        void WriteLine(string text);

        /// <summary>
        /// Writes a line of text to the file being generated, resolving the symbols between square
        /// brackets `[...]` along the way. Uses a default value when the resolve process fails.
        /// </summary>
        /// <param name="text"></param>
        /// <param name="fallbackValue"></param>
        void WriteLine(string text, string fallbackValue);

        /// <summary>
        /// Writes text to the file being generated as-is, without resolving anything.
        /// </summary>
        /// <param name="text">The text to write into the file.</param>
        void WriteVerbatim(string text);

        /// <summary>
        /// Writes a line of text to the file being generated as-is, without resolving anything.
        /// </summary>
        /// <param name="text">The text to write into the file.</param>
        void WriteLineVerbatim(string text);

        /// <summary>
        /// Declares one or several variables to be replaced while resolving.
        /// </summary>
        /// <param name="variables">An array of <see cref="VariableAssignment"/> instances that designate the variables to replace when resolving.</param>
        /// <returns>An object that implements <see cref="IDisposable"/> and will automatically undefine the variable when it's <see cref="IDisposable.Dispose"/> method is invoked.</returns>
        /// <exception cref="ArgumentNullException"><paramref name="variables"/> is null.</exception>
        IDisposable Declare(params VariableAssignment[] variables);

        /// <summary>
        /// Resolves environment variables in the generated file.
        /// </summary>
        /// <param name="platform">The platform whose <see cref="EnvironmentVariableResolver"/> is going to be used by the implementation.</param>
        /// <param name="variables">An array of <see cref="VariableAssignment"/> containing the variables to resolve.</param>
        /// <exception cref="ArgumentNullException"><paramref name="variables"/> is null.</exception>
        void ResolveEnvironmentVariables(Platform platform, params VariableAssignment[] variables);

        /// <summary>
        /// Check if the generated file is different than file on disk.
        /// </summary>
        /// <param name="file">file to check</param>
        /// <returns>true: file is different. False: File is the same</returns>
        bool IsFileDifferent(FileInfo file);

        /// <summary>
        /// Write the generated file if different than existing file on disk.
        /// </summary>
        /// <param name="file">file to write</param>
        /// <param name="bypassAutoCleanupDatabase">Cleanup database is used so that if a previously written file is no longer written it is automatically deleted.</param>
        /// <returns></returns>
        bool FileWriteIfDifferent(FileInfo file, bool bypassAutoCleanupDatabase = false);

        /// <summary>
        /// Writes the content of this generator to another generator using efficient stream copy.
        /// </summary>
        /// <param name="generator"></param>
        void WriteTo(IFileGenerator generator);

        /// <summary>
        /// This method clears the content of the generated file.
        /// </summary>
        void Clear();

        /// <summary>
        /// Check if the generator has been written to.
        /// </summary>
        bool IsEmpty();
    }

    /// <summary>
    /// Provides extension methods for <see cref="IFileGenerator"/>.
    /// </summary>
    public static class FileGeneratorExtensions
    {
        /// <summary>
        /// Declares one or several variables to be replaced while resolving.
        /// </summary>
        /// <param name="fileGenerator">The <see cref="IFileGenerator"/> instance to use.</param>
        /// <param name="identifier">The symbol's identifier name.</param>
        /// <param name="value">The value of the symbol when resolving.</param>
        /// <returns>An object that implements <see cref="IDisposable"/> and will automatically undefine the variable when it's <see cref="IDisposable.Dispose"/> method is invoked.</returns>
        /// <exception cref="ArgumentNullException"><paramref name="fileGenerator"/> or <paramref name="identifier"/> is null.</exception>
        public static IDisposable Declare(this IFileGenerator fileGenerator, string identifier, object value)
        {
            if (fileGenerator == null)
                throw new ArgumentNullException(nameof(fileGenerator));
            if (identifier == null)
                throw new ArgumentNullException(nameof(identifier));

            return fileGenerator.Declare(new VariableAssignment(identifier, value));
        }
    }
}
