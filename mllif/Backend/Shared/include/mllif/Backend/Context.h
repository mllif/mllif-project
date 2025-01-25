/*
 * Copyright 2025 Yeong-won Seo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

namespace mllif {
    class Error {
        std::string _what;

      public:
        Error(const std::string& what) : _what(what) {}

        const std::string& what() const { return _what; }
    };

    class MLLIFContext {
        std::vector<Error> _errors;

      public:
        void error(const std::string& what) {
            _errors.push_back(Error(what));
        }

        operator bool() const {
            return _errors.empty();
        }

        bool operator!() const {
            return !_errors.empty();
        }
    };
}
