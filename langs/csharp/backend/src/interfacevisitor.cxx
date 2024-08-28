/*
 * Copyright (C) 2024  Yeong-won Seo
 *
 * This file is part of LILAC.
 *
 * LILAC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3, or (at your option) any later
 * version.
 *
 * LILAC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "interfacevisitor.h"
#include "shared/char.h"

#define TAB "\t"
#define THIS "__ref"

static std::string Type(std::string src, const frxml::dom& loc)
{
    int refC = 0;
    for (; src[src.size() - 1 - refC] == '*'; refC++)
    {
    }
    if (refC)
        src = src.erase(src.length() - refC, refC);
    std::string ref(refC, '*');

    if (src == "__fp128")
        throw lilac::shared::exception("__fp128 is not supported by C#", loc);

    static std::map<std::string, std::string> builtins = {
        { "__void", "void" },
        { "__bool", "bool" },
        { "__u8", "byte" },
        { "__u16", "ushort" },
        { "__u32", "uint" },
        { "__uptr", "nuint" },
        { "__u64", "ulong" },
        { "__u128", "System.UInt128" },
        { "__s8", "sbyte" },
        { "__s16", "short" },
        { "__s32", "int" },
        { "__sptr", "nint" },
        { "__s64", "long" },
        { "__s128", "System.Int128" },
        { "__fp16", "System.Half" },
        { "__fp32", "float" },
        { "__fp64", "double" }
    };
    if (builtins.contains(src))
        return builtins[src] + ref;

    for (char& i: src)
        if (i == '/')
            i = '.';
    return src + ref;
}

static lilac::shared::GenericInterfaceVisitor<
    lilac::csharp::VisitContext,
    lilac::csharp::NamespaceVisitor,
    lilac::csharp::RecordVisitor,
    lilac::csharp::EnumVisitor,
    lilac::csharp::EnumConstantVisitor,
    lilac::csharp::FunctionVisitor,
    lilac::csharp::MethodVisitor,
    lilac::csharp::CtorVisitor,
    lilac::csharp::DtorVisitor
> s_GenericInterfaceVisitor;

void ForeachChildren(lilac::csharp::VisitContext& ctx, const frxml::dom& current, int depth, bool spacing)
{
    for (auto i = 0; i < current.children().size(); ++i)
    {
        if (spacing && i)
            ctx.Output << lilac::shared::endl;

        s_GenericInterfaceVisitor.Begin(ctx, current, current.children()[i], depth);
        s_GenericInterfaceVisitor.End(ctx, current, current.children()[i], depth);
    }
}

#pragma region AssemblyInterfaceVisitor

void lilac::csharp::CSharpAssemblyInterfaceVisitor::Begin(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    if (current.tag().view() != "assembly")
        throw shared::exception("AssemblyInterfaceVisitor should deal with assembly element only", current);

    const auto indent = shared::GetIndent(depth);

    auto addDepth = 0;
    if (!ctx.RootNamespace.empty())
    {
        ctx.Output
            << indent << "namespace " << ctx.RootNamespace << shared::endl
            << indent << "{" << shared::endl;
        addDepth++;
    }

    ForeachChildren(ctx, current, depth + addDepth, true);
}

void lilac::csharp::CSharpAssemblyInterfaceVisitor::End(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    if (ctx.RootNamespace.empty())
        return;

    const auto indent = shared::GetIndent(depth);
    ctx.Output << indent << "}" << shared::endl;
}

#pragma endregion

#pragma region NamespaceVisitor

std::string lilac::csharp::NamespaceVisitor::GetName() const
{
    return "namespace";
}

void lilac::csharp::NamespaceVisitor::Begin(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);

    ctx.Output
        << indent << "namespace " << current.attr().at("name").view() << shared::endl
        << indent << '{' << shared::endl;

    ForeachChildren(ctx, current, depth + 1, true);
}

void lilac::csharp::NamespaceVisitor::End(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);
    ctx.Output << indent << "}" << shared::endl;
}

#pragma endregion

#pragma region RecordVisitor

std::string lilac::csharp::RecordVisitor::GetName() const
{
    return "record";
}

void lilac::csharp::RecordVisitor::Begin(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);

    const auto size  = current.attr().at("size").view();
    const auto align = current.attr().at("align").view();

#define INTEROP_NS "global::System.Runtime.InteropServices."
#define LAYOUT_KIND INTEROP_NS "LayoutKind."
#define COMPILER_NS "global::System.Runtime.CompilerServices."

    ctx.Output
        << indent << "[" INTEROP_NS"StructLayout(" LAYOUT_KIND"Explicit, Size=" << size << ", Pack=" << align << ")]\n"
        << indent << "public struct InteropTest\n"
        << indent << "{\n"
        << indent << TAB "[" INTEROP_NS"FieldOffset(0)]\n"
        << indent << TAB "private byte " THIS ";\n"
        << indent << "\n"
        << indent << TAB "private IntPtr This\n"
        << indent << TAB "{\n"
        << indent << TAB TAB "get\n"
        << indent << TAB TAB "{\n"
        << indent << TAB TAB TAB "unsafe\n"
        << indent << TAB TAB TAB "{\n"
        << indent << TAB TAB TAB TAB "return (IntPtr)" COMPILER_NS"Unsafe.AsPointer(ref __ref);\n"
        << indent << TAB TAB TAB "}\n"
        << indent << TAB TAB "}\n"
        << indent << TAB "}\n";

#undef COMPILER_NS
#undef LAYOUT_KIND
#undef INTEROP_NS

    if (!current.children().empty())
        ctx.Output << shared::endl;

    ForeachChildren(ctx, current, depth + 1, true);
}

void lilac::csharp::RecordVisitor::End(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);
    ctx.Output << indent << "}" << shared::endl;
}

#pragma endregion

#pragma region EnumVisitor

std::string lilac::csharp::EnumVisitor::GetName() const
{
    return "enum";
}

void lilac::csharp::EnumVisitor::Begin(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);

    const auto name = current.attr().at("name").view();
    const auto type = current.attr().at("type").view();

    ctx.Output
        << indent << "public enum " << name << " : " << Type(static_cast<std::string>(type), current) << "\n"
        << indent << "{\n";

    ForeachChildren(ctx, current, depth + 1, false);
}

void lilac::csharp::EnumVisitor::End(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);
    ctx.Output << indent << "}" << shared::endl;
}

#pragma endregion

#pragma region EnumConstantVisitor

std::string lilac::csharp::EnumConstantVisitor::GetName() const
{
    return "constant";
}

void lilac::csharp::EnumConstantVisitor::Begin(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    if (parent.tag().view() != EnumVisitor().GetName())
        throw shared::exception("`constant' element should be child of `enum' element", current);

    const auto indent = shared::GetIndent(depth);

    const auto name  = current.attr().at("name").view();
    const auto value = current.attr().at("value").view();

    ctx.Output << indent << name << " = " << value << ',' << shared::endl;
}

void lilac::csharp::EnumConstantVisitor::End(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
}

#pragma endregion

std::string lilac::csharp::FunctionVisitor::GetName() const
{
    return "function";
}

void lilac::csharp::FunctionVisitor::Begin(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
    const auto indent = shared::GetIndent(depth);

    auto isUnsafe = false;


    const auto returnType = Type(static_cast<std::string>(current.attr().at("return").view()), current);
    // ReSharper disable once CppDFAConstantConditions
    isUnsafe = isUnsafe || returnType.contains('*');

    // parameters

    std::stringstream managedParams;
    for (auto i = 0; i < current.children().size(); ++i)
    {
        if (i) managedParams << ", ";

        auto param = current.children().at(i);
        auto type  = Type(static_cast<std::string>(param.attr().at("type").view()), current.children()[i]);
        managedParams << type << ' ' << param.attr().at("name").view();
    }

    std::stringstream nativeParams;
    for (auto i = 0; i < current.children().size(); ++i)
    {
        if (i) nativeParams << ", ";

        auto param = current.children().at(i);
        auto type  = Type(static_cast<std::string>(param.attr().at("type").view()), current.children()[i]);
        if (!isUnsafe && type.contains('*'))
            isUnsafe = true;
        if (type == "bool")
            nativeParams << "[System.Runtime.InteropServices.MarshalAs(System.Runtime.InteropServices.UnmanagedType.U1)] ";
        nativeParams << type << ' ' << param.attr().at("name").view();
    }

    std::stringstream paramRef;
    for (auto i = 0; i < current.children().size(); ++i)
    {
        if (i) paramRef << ", ";
        paramRef << current.children().at(i).attr().at("name").view();
    }

    // callconv

#define CALLCONV "System.Runtime.InteropServices.CallingConvention."
    static std::map<std::string_view, std::string_view> callconvs = {
        { "cdecl", CALLCONV "Cdecl" },
        { "stdcall", CALLCONV "StdCall" },
        { "thiscall", CALLCONV "ThisCall" },
    };
#undef CALLCONV

    auto callconv = current.attr().at("callconv").view();
    if (!callconvs.contains(callconv))
        throw shared::exception(std::format("Calling convention `{}' is not supported", callconv), current);
    callconv = callconvs.at(callconv);

    // assembly - managed parameters

    const auto isStatic = current.tag().view() == FunctionVisitor().GetName();

    const auto isMemberFn = parent.tag().view() == RecordVisitor().GetName();
    if (!isMemberFn && !isStatic)
        throw shared::exception("'element' can only be member function", current);

    const auto name = current.attr().at("name").view();

    ctx.Output << indent << "public ";
    if (!isMemberFn)
        ctx.Output << "static ";
    if (isUnsafe)
        ctx.Output << "unsafe ";
    ctx.Output << returnType << ' ' << name << '(' << managedParams.str() << ')' << shared::endl;

    // assembly - body

    ctx.Output << indent << "{" << shared::endl;

    ctx.Output << indent << TAB;
    if (returnType != "void")
        ctx.Output << "return ";

    if (isMemberFn)
    {
        ctx.Output << "__PInvoke(This";
        if (!paramRef.str().empty())
            ctx.Output << ", " << paramRef.str();
        ctx.Output << ");" << shared::endl;
    }
    else
    {
        ctx.Output << "__PInvoke(" << paramRef.str() << ");" << shared::endl;
    }

    ctx.Output << shared::endl;

    // assembly - P/Invoke def

    const auto mangling = current.attr().at("mangle").view();

#define INTEROP_NS "System.Runtime.InteropServices."
    ctx.Output
        << indent << TAB "[" INTEROP_NS "DllImport(" << shared::endl
        << indent << TAB TAB "\"" << ctx.LibraryName << "\"," << shared::endl
        << indent << TAB TAB "EntryPoint = \"" << mangling << "\"," << shared::endl
        << indent << TAB TAB "CallingConvention = " << callconv << ',' << shared::endl
        << indent << TAB TAB "ExactSpelling = true)]" << shared::endl;
#undef INTEROP_NS

    ctx.Output << indent << TAB "static extern " << returnType << " __PInvoke(";
    if (isMemberFn)
    {
        ctx.Output << "IntPtr @this";
        if (!nativeParams.str().empty())
            ctx.Output << ", ";
    }
    ctx.Output << nativeParams.str() << ");" << shared::endl;

    ctx.Output << indent << "}" << shared::endl;
}

void lilac::csharp::FunctionVisitor::End(
    VisitContext&     ctx,
    const frxml::dom& parent,
    const frxml::dom& current,
    int               depth)
{
}

std::string lilac::csharp::MethodVisitor::GetName() const
{
    return "method";
}

std::string lilac::csharp::CtorVisitor::GetName() const
{
    return "ctor";
}

std::string lilac::csharp::DtorVisitor::GetName() const
{
    return "dtor";
}
